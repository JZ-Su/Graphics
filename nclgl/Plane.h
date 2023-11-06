#pragma once
#include "Vector3.h"

class Plane
{
public:
	Plane(void) {};
	Plane(const Vector3& normal, float distance, bool normalise = false);
	~Plane(void) {};

	void SetNormal(const Vector3& n) { this->normal = n; }
	Vector3 GetNormal() const { return normal; }

	void SetDistance(float d) { distance = d; }
	float GetDistance() const { return distance; }

	bool SphereInPlane(const Vector3& position, float radius) const;
protected:
	Vector3 normal;
	float distance;
};

