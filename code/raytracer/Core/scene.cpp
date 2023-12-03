/****************************************************************************
*
* krt - Ken's Raytracer - Coursework Edition. (C) Copyright 1993-2022.
*
* I've put a lot of time and effort into this code. For the last decade
* it's been used to introduce hundreds of students at multiple universities
* to raytracing. It forms the basis of your coursework but you are free
* to continue using/developing forever more. However, I ask that you don't
* share the code or your derivitive versions publicly. In order to continue
* to be used for coursework and in particular assessment it's important that
* versions containing solutions are not searchable on the web or easy to
* download.
*
* If you want to show off your programming ability, instead of releasing
* the code, consider generating an incredible image and explaining how you
* produced it.
*/

#include "scene.h"

Scene::Scene()
{
	object_list = 0;
	light_list = 0;
	
}

bool Scene::shadowtrace(Ray ray, float limit)
{
	Object *objects = this->object_list;

	while (objects != 0)
	{
		Hit *hit = this->select_first(objects->intersection(ray));

		if (hit != 0)
		{
		  if ((hit->t > 0.00000001f) &&( hit->t < limit))
		    {
			  delete hit;
		      return true;
		    }
		  delete hit;
		}

		objects = objects->next;
	}

	return false;
}

Hit *Scene::trace(Ray ray)
{
	Hit *best_hit = 0;

	Object *objects = this->object_list;

	while (objects != 0)
	{
		Hit *hit = this->select_first(objects->intersection(ray));

		if (hit != 0)
		{
			if (best_hit == 0)
			{
				best_hit = hit;

			} else if (hit->t < best_hit->t)
			{
				delete best_hit;
				best_hit = hit;
			}
			else
			{
				delete hit;
			}
		}

		objects = objects->next;
	}

	return best_hit;
}

Hit* Scene::select_first(Hit* list)
{
	Hit* result = 0;

	while (list != 0)
	{
		if (list->t >= 0.0f)
		{
			result = list;
			list = list->next;
			break;
		}

		Hit* temp = list;
		list = list->next;
		delete temp;
	}

	while (list != 0)
	{
		Hit* temp = list;
		list = list->next;
		delete temp;
	}

	return result;
}

void Scene::raytrace(Ray ray, int recurse, Colour &colour, float &depth)
{
	Object *objects = object_list;
	Light *lights = light_list;
		
	// a default colour if we hit nothing.
	colour.r = 0.0f;
	colour.g = 0.0f;
	colour.b = 0.0f;
	colour.a = 0.0f;
	depth = 0.0f;

	// first step, find the closest primitive
	Hit *best_hit = this->trace(ray);

	// if we found a primitive then compute the colour we should see
	if (best_hit != 0)
	{
		depth = best_hit->t;
		colour = colour + best_hit->what->material->compute_once(ray, *best_hit, recurse); // this will be the global components such as ambient or reflect/refract
		
		if ((recurse < 4) && best_hit->normal.len_sqr() > 0){
			Colour indirectIllumination = calculateIndirectIllumination(best_hit->position);
        	colour = colour + indirectIllumination;
		}
		
		// next, compute the light contribution for each light in the scene.
		Light* light = light_list;
		while (light != (Light*)0)
		{
			Vector viewer;
			Vector ldir;

			viewer = -best_hit->position;
			viewer.normalise();

			bool lit;
			lit = light->get_direction(best_hit->position, ldir);

			if (ldir.dot(best_hit->normal) > 0)
			{
				lit = false;//light is facing wrong way.
			}

			// Put the shadow check here, if lit==true and in shadow, set lit=false
			if (lit)
			{
				float shadowRayStartOffset =0.0001f;
				float shadowLimit = 1000000000.0f;
				Ray shadow_ray;

				shadow_ray.direction = -ldir;
				// Adjust starting point of shadow ray to start just outside the object to stop self intersection
				shadow_ray.position = best_hit->position + (shadowRayStartOffset * shadow_ray.direction);

				if (this->shadowtrace(shadow_ray, shadowLimit))
				{
					lit = false; //there's a shadow so no lighting, if realistically close
				}
			}

			if (lit)
			{
				Colour intensity;
				
				light->get_intensity(best_hit->position, intensity);
				
				colour = colour + intensity * best_hit->what->material->compute_per_light(viewer, *best_hit, ldir); // this is the per light local contrib e.g. diffuse, specular

			}

			light = light->next;
		}

		delete best_hit;
	} else
		{
			colour.r = 0.0f;
			colour.g = 0.0f;
			colour.b = 0.0f;
			colour.a = 1.0f;
		}
}

void Scene::add_object(Object *obj)
{
  obj->next = this->object_list;
  this->object_list = obj;
}

void Scene::add_light(Light *light)
{
  light->next = this->light_list;
  this->light_list = light;
}

void Scene::photontrace(Photon photon, int recurse){
    // Perform intersection test to find the nearest surface
	Ray ray;
	ray.position = Vertex(photon.position.x, photon.position.y, photon.position.z);
	ray.direction = photon.direction;
    Hit* hit = trace(ray);

    if (hit && hit->normal.len_sqr() > 0) {
        // Store photon information at the intersection point
		photon.position = hit->position;
        
        // Update the photon direction based on material properties
		Photon newPhoton = Photon(photon.position, photon.direction, photon.energy, photon.type);
        bool storePhoton = hit->what->material->receivePhoton(&newPhoton, *hit);

		if (storePhoton){
			photonMap->addPhoton(photon);
			if (photon.type == caustic){
				causticMap->addPhoton(photon);
			}
		}

		if (!newPhoton.absorbed && recurse > 0){
			photontrace(newPhoton, recurse - 1);
		}
    }

	delete hit;
}

void Scene::photonMapping(int n){
	photonMap = new PhotonMap();
	causticMap = new PhotonMap();

	//Trace Photons
	Light* light = light_list;
	while (light != (Light*)0)
	{
		vector<Photon> lightPhotons = light->getPhotons(n);
		int f = lightPhotons.size();
		for (int i = 0; i < lightPhotons.size(); i++){
			photontrace(lightPhotons[i], 5);
			if (i%(n / 20) == 0){
				std::cout << "\nLight:" <<(100 * i/n) << "%";
			}
		}

		light = light->next;
	}

	// Build Photon Maps
	photonMap->buildMap();
	causticMap->buildMap();
}

Colour Scene::calculateIndirectIllumination(Vector point){
	Colour result;
	float n = 20;
	vector<Photon> nearbyPhotons = photonMap->query(point, n);

	float radius = 0.0f;
	for (Photon photon : nearbyPhotons){
		if ((photon.position - point).len_sqr() > pow(radius, 2)){
			radius = (photon.position - point).length();
		}
	}
	
	Colour flux = Colour();
	for (Photon photon : nearbyPhotons){
		flux = flux + photon.energy;
	}

	result = flux * (1 / (M_PI * pow(radius,2)));

	return result;

	// if (nearbyPhotons.size() > 0){
	// 	result = Colour(0.3f, 0.3f, 0.3f);
	// }

	
}

