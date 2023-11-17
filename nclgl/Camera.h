#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public:
	Camera(void) {
		position = Vector3(0, 0, 0);
		yaw   = 0.0f;
		pitch = 0.0f;
		roll  = 0.0f;
		previousYaw      = yaw;
		previousPitch    = pitch;
		previousPosition = position;
	};
	Camera(float pitch, float yaw, float roll, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
		this->position = position;
		previousYaw = yaw;
		previousPitch = pitch;
	};
	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f, float speed = 100.0f);

	Matrix4 BuildViewMatrix();

	Vector3	 GetPosition() const		{ return position; }
	void	 SetPosition(Vector3 val)	{ position = val; }

	float	GetYaw() const		{ return yaw; }
	void	SetYaw(float y)		{ yaw = y; }

	float	GetPitch() const	{ return pitch; }
	void	SetPitch(float p)	{ pitch = p; }

	float   GetRoll() const     { return roll; }
	void    SetRoll(float r)    { roll = r; }

	float   GetDeltaYaw() const      { return yaw - previousYaw; }
	float   GetDeltaPitch() const    { return pitch - previousPitch; }
	Vector3 GetDeltaPosition() const { return position - previousPosition; }
	Vector3 GetPrePosition() const   { return previousPosition; }

	void SetCamera(Vector3 position, float pitch, float yaw, float roll);

private:
	float yaw;
	float pitch;
	float roll;
	Vector3 position;

	float   previousYaw;
	float   previousPitch;
	Vector3 previousPosition;
};