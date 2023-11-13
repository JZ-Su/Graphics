#pragma once
#include "Mesh.h"

class Sphere : public Mesh {
public:
	Sphere(float degree,int texRepeat, bool inner);
	~Sphere(void);

	static Sphere* GenHeightMap();
	static Sphere* GenWaterWave(float degree, int texRepeat);

	void Update(float dt);

protected:
	float radius;
	float degree;

	Vector3 PolorToCartesianCoord(Vector3 polorCoord);
	Vector3 CartesianToPolorCoord(Vector3 cartesianCoord);
};

