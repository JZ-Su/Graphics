#include "Sphere.h"

Sphere::Sphere(float degree,int texRepeat, bool inner) {
	this->degree = degree;
	int theta = 180 / degree;
	int phi = 360 / degree;
	radius = 1.0;
	numVertices = (theta + 1) * (phi +1);
	numIndices = (theta + 2) * (phi + 2) * 6;
	vertices = new Vector3[numVertices];
	indices = new GLuint[numIndices];
	textureCoords = new Vector2[numVertices];

	for (int z = 0; z <= phi; z++) {
		for (int x = 0; x <= theta; x++) {
			int offset = z * (theta + 1) + x;
			vertices[offset] = PolorToCartesianCoord(Vector3(radius, (float)x * PI / (float)theta, (float)z * PI / (float)theta));
			if (texRepeat == 2) {
				if (z < phi / 2) {
					textureCoords[offset] = Vector2((float)x / (float)theta, 2.0 * (float)z / (float)phi);
				}
				else {
					textureCoords[offset] = Vector2((float)x / (float)theta, 2.0 - 2.0 * (float)z / (float)phi);
				}
			}
			else {
				textureCoords[offset] = Vector2((float)texRepeat * (float)x / (float)theta, (float)texRepeat * (float)z / (float)phi);
			}
		}
	}

	int i = 0;
	for (int z = 0; z <= phi + 1; z++) {
		for (int x = 0; x < theta ; x++) {
			int a = z * theta + x;
			int b = a + theta + 1;
			int c = a + theta + 2;
			int d = a + 1;

			if (inner) {
				indices[i++] = a;
				indices[i++] = d;
				indices[i++] = b;

				indices[i++] = d;
				indices[i++] = c;
				indices[i++] = b;
			}
			else {
				indices[i++] = a;
				indices[i++] = b;
				indices[i++] = d;

				indices[i++] = b;
				indices[i++] = c;
				indices[i++] = d;
			}
		}
	}
	BufferData();
}

Sphere::~Sphere(void) {}

void Sphere::GenHeightMap() {
	int theta = 180 / degree;
	int phi = 360 / degree;
	for (int z = 2; z < phi - 1; z++) {
		for (int x = 2; x < theta - 1; x++) {
			int offset = z * (theta + 1) + x;
			vertices[offset].x += 0.15 * sin(10 * x);
			vertices[offset].y += 0.15 * cos(5 * z);
			vertices[offset].z += 0.15 * sin(x);
		}
	}
	BufferData();
}

void Sphere::GenWaterWave() {
	int theta = 180 / degree;
	int phi = 360 / degree;
	for (int z = 2; z < phi - 1; z++) {
		for (int x = 2; x < theta - 1; x++) {
			int offset = z * (theta + 1) + x;
			Vector3 polorCoord = CartesianToPolorCoord(vertices[offset]);
			switch ((z + x) % 4) {
			case 0:  polorCoord.x = 0.98;
			case 2:	 polorCoord.x = 1.02;
			default: break;
			}
			vertices[offset] = PolorToCartesianCoord(polorCoord);
		}
	}
	BufferData();
}

void Sphere::Update(float totalTime) {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteVertexArrays(1, &arrayObject);
	int theta = 180 / degree;
	int phi = 360 / degree;
	for (int z = 2; z < phi - 1; z++) {
		for (int x = 2; x < theta - 1; x++) {
			int offset = z * (theta + 1) + x;
			Vector3 polorCoord = CartesianToPolorCoord(vertices[offset]);
			switch ((z + x) % 4) {
			case 0: polorCoord.x += 0.00001 * sin(totalTime + PI / 2);		break;
			case 1: polorCoord.x += 0.00001 * sin(totalTime + PI);			break;
			case 2: polorCoord.x += 0.00001 * sin(totalTime + PI * 3 / 2);	break;
			case 3: polorCoord.x += 0.00001 * sin(totalTime);				break;
			default: break;
			}
			vertices[offset] = PolorToCartesianCoord(polorCoord);
		}
	}
	BufferData();
}

Vector3 Sphere::PolorToCartesianCoord(Vector3 polorCoord) {
	Vector3 coord = Vector3();
	coord.x = polorCoord.x * sin(polorCoord.y) * cos(polorCoord.z);
	coord.y = polorCoord.x * cos(polorCoord.y);
	coord.z = polorCoord.x * sin(polorCoord.y) * sin(polorCoord.z);
	return coord;
}

Vector3 Sphere::CartesianToPolorCoord(Vector3 cartesianCoord) {
	Vector3 coord = Vector3();
	coord.x = sqrt(cartesianCoord.x * cartesianCoord.x + cartesianCoord.y * cartesianCoord.y + cartesianCoord.z * cartesianCoord.z);
	coord.y = acos(cartesianCoord.y / coord.x);
	coord.z = atan2(cartesianCoord.z , cartesianCoord.x);
	return coord;
}