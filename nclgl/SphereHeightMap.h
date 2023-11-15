#pragma once
#include "Sphere.h"
#include <vector>

class SphereHeightMap : public Sphere {
public:
	SphereHeightMap();
	~SphereHeightMap() {};

	std::vector<Mesh*> GetMesh() const { return heightMesh; }

protected:
	Mesh* GenSmallMesh(Vector3 A, Vector3 C, Vector3 B, Vector2 texCoordA, Vector2 texCoordC, Vector2 texCoordB, bool isPole);
	Vector3 ResetRadius(Vector3 CartesianCoord, float dRadius);
	std::vector<Mesh*> heightMesh;

};