#pragma once
#include "Mesh.h"

class Sphere : public Mesh {
public:
	Sphere(float degree, bool inner);
	~Sphere(void);

	void GenHeightMap();
	void GenWaterWave();

	void Update(float dt);

protected:
	float radius;
	float degree;

	Vector3 PolorToCartesianCoord(Vector3 polorCoord);
	Vector3 CartesianToPolorCoord(Vector3 cartesianCoord);
};

