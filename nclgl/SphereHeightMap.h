#pragma once
#include "Sphere.h"
#include <vector>

class SphereHeightMap : public Sphere {
public:
	SphereHeightMap();
	~SphereHeightMap() {};

	std::vector<Mesh*>::const_iterator GetMeshIteratorStart() { return heightMesh.begin(); }
	std::vector<Mesh*>::const_iterator GetMeshIteratorEnd() { return heightMesh.end(); }

	void Draw();

protected:
	std::vector<Mesh*> heightMesh;
};