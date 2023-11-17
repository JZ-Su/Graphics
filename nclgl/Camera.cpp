#include "Camera.h"
#include "Window.h"
#include <algorithm>
void Camera::UpdateCamera(float dt, float speed) {
	previousPosition = position;
	previousPitch    = pitch;
	previousYaw      = yaw;

	if (Window::GetMouse()->ButtonHeld(MOUSE_LEFT)) {
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
		roll -= 100.0f * dt;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) {
		roll += 100.0f * dt;
	}
	if (roll < 0) {
		roll += 360.0f;
	}
	if (roll > 360.0f) {
		roll -= 360.0f;
	}

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-roll, Vector3(0, 0, -1)) * Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	Vector3 up = rotation * Vector3(0, 1, 0);

	speed = speed * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position += up * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position -= up * speed;
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return  Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-yaw, Vector3(0, 1, 0))
		 * Matrix4::Rotation(-roll, Vector3(0, 0, 1)) * Matrix4::Translation(-position);
}

void Camera::SetCamera(Vector3 position, float pitch, float yaw, float roll) {
	this->previousYaw      = this->yaw;
	this->previousPitch    = this->pitch;
	this->previousPosition = this->position;
	this->position = position;
	this->pitch    = pitch;
	this->yaw      = yaw;
	this->roll     = roll;
}