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

#include "quadratic_object.h"
#include <math.h>

using namespace std;

Quadratic::Quadratic(float p_a, float p_b, float p_c, float p_d, 
	float p_e, float p_f, float p_g, float p_h, float p_i, float p_j) {
	next = (Object *)0;

	// Create matrix to store the quadratic coefficients
	transform = Transform(p_a, p_b, p_c, p_d, p_b, p_e, 
		p_f, p_g, p_c, p_f, p_h, p_i, p_d, p_g, p_i, p_j);
}

Hit *Quadratic::intersection(Ray ray)
{
	float a = transform.matrix[0][0];
    float b = transform.matrix[0][1];
    float c = transform.matrix[0][2];
    float d = transform.matrix[0][3];
    float e = transform.matrix[1][1];
    float f = transform.matrix[1][2];
    float g = transform.matrix[1][3];
    float h = transform.matrix[2][2];
    float i = transform.matrix[2][3];
    float j = transform.matrix[3][3];

	Vertex P = ray.position;
	Vector D = ray.direction;
	D.normalise();

	// Calculate coefficients for equation solving for t

	float A = a * pow(D.x, 2)+ 
		2.0 * b * D.x * D.y +
		2.0 * c * D.x * D.z +
		e * pow(D.y, 2) +
		2.0 * f * D.y * D.z +
		h * pow(D.z, 2);

	float B = 2.0 * (a * P.x * D.x +
		b * (P.x * D.y + D.x * P.y) +
		c * (P.x * D.z + D.x * P.z) +
		d * D.x +
		e * P.y * D.y +
		f * (P.y * D.z + D.y * P.z) +
		g * D.y +
		h * P.z * D.z +
		i * D.z);

	float C = a * pow(P.x, 2) +
		2.0 * b * P.x * P.y +
		2.0 * c * P.x * P.z +
		2.0 * d * P.x +
		e * pow(P.y, 2) +
		2.0 * f * P.y * P.z +
		2.0 * g * P.y +
		h * pow(P.z, 2) +
		2.0 * i * P.z +
		j;


	float discriminant = pow(B, 2) - (4.0 * A * C);

	if (discriminant <= 0){
		// No real roots, no intersection OR tangent
		return 0;
	}
	else{
		// First hit
		float t0 = (-B - sqrt(discriminant)) / (2.0 * A);

		if (t0 < 0){
			// Intersection is behind origin of ray, so no first intersection

			// Check for second
			float t1 = (-B + sqrt(discriminant)) / (2.0 * A);
			if (t1 < 0){
				// No intersection
				return 0;
			}
			
			// There is a second intersection, so calculate
			Vertex position = P + t1 * D;

			// Calculate normal
			Vector normal;
			normal.x = a * position.x + b * position.y + c * position.z + d;
			normal.y = b * position.x + e * position.y + f * position.z + g;
			normal.z = c * position.x + f * position.y + h * position.z + i;

			// If normal pointing wrong way, flip it
			if (normal.dot(ray.direction) > 0.0)
			{
				normal.negate();
			}
			normal.normalise();
			
			Hit* hit = new Hit();
			hit->entering = false;
			hit->t = t1;
			hit->position = position;
			hit->normal = normal;
			hit->what = this;
			hit->next = 0;
			return hit;
		}
		else{
			Vertex position1 = P + t0 * D;

			// Calculate normal
			Vector normal1;
			normal1.x = a * position1.x + b * position1.y + c * position1.z + d;
			normal1.y = b * position1.x + e * position1.y + f * position1.z + g;
			normal1.z = c * position1.x + f * position1.y + h * position1.z + i;

			// If normal pointing wrong way, flip it
			if (normal1.dot(ray.direction) > 0.0)
			{
				normal1.negate();
			}
			normal1.normalise();
			
			Hit* hit1 = new Hit();
			Hit* hit2 = new Hit();
			hit1->entering = true;
			hit1->t = t0;
			hit1->position = position1;
			hit1->normal = normal1;
			hit1->what = this;


			// Calculate second intersection point
			float t1 = (-B + sqrt(discriminant)) / (2.0 * A);
			Vertex position2 = P + t1 * D;

			// Calculate normal
			Vector normal2;
			normal2.x = a * position2.x + b * position2.y + c * position2.z + d;
			normal2.y = b * position2.x + e * position2.y + f * position2.z + g;
			normal2.z = c * position2.x + f * position2.y + h * position2.z + i;

			// If normal pointing wrong way, flip it
			if (normal2.dot(ray.direction) > 0.0)
			{
				normal2.negate();
			}
			normal2.normalise();

			hit2->entering = false;
			hit2->t = t1;
			hit2->position = position2;
			hit2->normal = normal2;
			hit2->what = this;

			hit1->next = hit2;
			hit2->next = 0;
			return hit1;
		}
	}
}


void Quadratic::apply_transform(Transform& trans)
{
	transform = trans.transpose() * transform * trans;
}