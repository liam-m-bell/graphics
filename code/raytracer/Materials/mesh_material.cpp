#include <math.h>

#include "mesh_material.h"
#include "../Materials/compound_material.h"
#include "../Materials/phong_material.h"
#include "../Materials/global_material.h"
#include "../Objects/polymesh_object.h"


// The compute_once() method supplies the ambient term.
Colour MeshMaterial::compute_once(Ray& viewer, Hit& hit, int recurse)
{
	PolyMesh *mesh = (PolyMesh*)(hit.what);
	return mesh->getFaceMaterial(hit.material)->compute_once(viewer, hit, recurse);
}

// The compute_per_light() method supplies the diffuse and specular terms.
Colour MeshMaterial::compute_per_light(Vector& viewer, Hit& hit, Vector& ldir)
{
	PolyMesh *mesh = (PolyMesh*)(hit.what);
	return mesh->getFaceMaterial(hit.material)->compute_per_light(viewer, hit, ldir);
}

