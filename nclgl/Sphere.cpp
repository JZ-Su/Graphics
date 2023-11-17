#include "Sphere.h"

Sphere::Sphere(float degree, int texRepeat) {
	this->degree = degree;
	int theta = 180 / degree;
	int phi = 360 / degree;
	radius = 1.0;
	numVertices = (theta + 1) * (phi + 2);
	numIndices = theta * (phi + 1) * 6;
	vertices = new Vector3[numVertices];
	indices = new GLuint[numIndices];
	textureCoords = new Vector2[numVertices];

	for (int z = 0; z <= phi + 1; z++) {
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
	for (int z = 0; z <= phi; z++) {
		for (int x = 0; x < theta; x++) {
			int a = z * (theta + 1) + x;
			int b = a + theta + 1;
			int c = a + theta + 2;
			int d = a + 1;

			indices[i++] = a;
			indices[i++] = b;
			indices[i++] = d;

			indices[i++] = b;
			indices[i++] = c;
			indices[i++] = d;
		}
	}
	GenerateNormals();
	GenerateTangents();
	BufferData();
}

Sphere::~Sphere(void) {}

//Sphere* Sphere::GenHeightMap() {
//	Sphere* s = new Sphere(15.0, 16);
//	int theta = 180 / s->degree;
//	int phi = 360 / s->degree;
//	float hei = s->radius;
//	for (int z = 0; z <= phi + 1; z++) {
//		for (int x = 0; x <= theta; x++) {
//			int offset = z * (theta + 1) + x;
//			if (offset % 13 == 0) {
//				hei = 1.1;
//			}
//			switch (offset) {
//			case 1:   case 10:  case 14:  case 23:  case 28:  case 36:  case 41:  case 50:  case 53:  case 56:  case 63:
//			case 67:  case 69:  case 76:  case 80:  case 83:  case 89:  case 93:  case 96:  case 102: case 105: case 115:
//			case 118: case 123: case 128: case 131: case 135: case 140: case 144: case 148: case 154: case 157: case 168:
//			case 170: case 181: case 183: case 194: case 196: case 207: case 209: case 220: case 222: case 233: case 246:
//			case 248: case 258: case 261: case 267: case 271: case 274: case 280: case 284: case 287: case 293: case 298:
//			case 300: case 307: case 311: case 313: case 324: case 326: case 335: hei = 1.1; break;
//
//			case 5:   case 19:  case 30:  case 44:  case 55:  case 60:  case 68:  case 73:  case 81:  case 84: 
//			case 94:  case 97:  case 106: case 119: case 125: case 133: case 138: case 147: case 152: case 160:
//			case 174: case 186: case 198: case 210: case 223: case 249: case 262: case 269: case 276: case 282:
//			case 290: case 295: case 304: case 308: case 316: case 330: case 235: hei = 0.9; break;
//			
//			default:
//				break;
//			}
//			if (hei == 1.0) continue;
//			Vector3 polorCoord = s->CartesianToPolorCoord(s->vertices[offset]);
//			polorCoord.x = hei;
//			s->vertices[offset] = s->PolorToCartesianCoord(polorCoord);
//		}
//	}
//	s->GenerateNormals();
//	s->GenerateTangents();
//	s->BufferData();
//	return s;
//}

Sphere* Sphere::GenWaterWave(float degree, int texRepeat) {
	Sphere* s = new Sphere(degree, texRepeat);
	int theta = 180 / degree;
	int phi = 360 / degree;
	for (int z = 2; z < phi - 1; z++) {
		for (int x = 0; x < theta - 1; x++) {
			int offset = z * (theta + 1) + x;
			Vector3 polorCoord = s->CartesianToPolorCoord(s->vertices[offset]);
			if (x == 0 || x == 1) {
				polorCoord.x = 1.01;
				s->vertices[offset] = s->PolorToCartesianCoord(polorCoord);
			}
			else {
				switch ((z + x) % 4) {
				case 0:  polorCoord.x = 0.98;
				case 2:	 polorCoord.x = 1.02;
				default: break;
				}
				s->vertices[offset] = s->PolorToCartesianCoord(polorCoord);
			}
		}
	}
	s->GenerateNormals();
	s->GenerateTangents();
	s->BufferData();
	return s;
}

void Sphere::Update(float totalTime) {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteVertexArrays(1, &arrayObject);
	int theta = 180 / degree;
	int phi = 360 / degree;
	for (int z = 2; z < phi - 1; z++) {
		for (int x = 0; x < theta - 1; x++) {
			int offset = z * (theta + 1) + x;
			Vector3 polorCoord = CartesianToPolorCoord(vertices[offset]);
			if (x == 0 || x == 1) {
				polorCoord.x += 0.00005 * (sin(totalTime + PI / 2));
				vertices[offset] = PolorToCartesianCoord(polorCoord);
			}
			else {
				switch ((z + x) % 4) {
				case 0: polorCoord.x += 0.00025 * sin(totalTime + PI / 2);		break;
				case 1: polorCoord.x += 0.00025 * sin(totalTime + PI);			break;
				case 2: polorCoord.x += 0.00025 * sin(totalTime + PI * 3 / 2);	break;
				case 3: polorCoord.x += 0.00025 * sin(totalTime);				break;
				default: break;
				}
				vertices[offset] = PolorToCartesianCoord(polorCoord);
			}
		}
	}
	GenerateNormals();
	GenerateTangents();
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
	coord.z = atan2(cartesianCoord.z, cartesianCoord.x);
	return coord;
}