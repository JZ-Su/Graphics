#include "SphereHeightMap.h"

SphereHeightMap::SphereHeightMap() : Sphere(15.0, 8) {
	int theta = 180 / this->degree;
	int phi = 360 / this->degree;
	float hei = this->radius;
	for (int z = 0; z <= phi + 1; z++) {
		for (int x = 0; x <= theta; x++) {
			int offset = z * (theta + 1) + x;
			if (offset % 13 == 0) {
				hei = 1.1;
			}
			switch (offset) {
			case 1:   case 10:  case 14:  case 23:  case 28:  case 36:  case 41:  case 50:  case 53:  case 56:  case 63:
			case 67:  case 69:  case 76:  case 80:  case 83:  case 89:  case 93:  case 96:  case 102: case 105: case 115:
			case 118: case 123: case 128: case 131: case 135: case 140: case 144: case 148: case 154: case 157: case 168:
			case 170: case 181: case 183: case 194: case 196: case 207: case 209: case 220: case 222: case 233: case 246:
			case 248: case 258: case 261: case 267: case 271: case 274: case 280: case 284: case 287: case 293: case 298:
			case 300: case 307: case 311: case 313: case 324: case 326: case 335: hei = 1.1; break;

			case 5:   case 19:  case 30:  case 44:  case 55:  case 60:  case 68:  case 73:  case 81:  case 84:
			case 94:  case 97:  case 106: case 119: case 125: case 133: case 138: case 147: case 152: case 160:
			case 174: case 186: case 198: case 210: case 223: case 249: case 262: case 269: case 276: case 282:
			case 290: case 295: case 304: case 308: case 316: case 330: case 235: hei = 0.9; break;

			default:
				break;
			}
			if (hei == 1.0) continue;
			Vector3 polorCoord = this->CartesianToPolorCoord(this->vertices[offset]);
			polorCoord.x = hei;
			this->vertices[offset] = this->PolorToCartesianCoord(polorCoord);
		}
	}
	int triCount = this->GetTriCount();
	for (int i = 0; i < triCount; i++) {
		if (i % 24 == 0 || (i - 1) % 24 == 0) continue; // They are repeated.
		unsigned int a = 0;
		unsigned int b = 0;
		unsigned int c = 0;
		GetVertexIndicesForTri(i, a, b, c);
		Vector3 A = vertices[a];
		Vector3 B = vertices[b];
		Vector3 C = vertices[c];
		Vector2 coordA = textureCoords[a];
		Vector2 coordB = textureCoords[b];
		Vector2 coordC = textureCoords[c];
		Mesh* m = new Mesh();
		
		/* Tri:  b-a
				 |\|
				 c-d  */
		// north pole, Tri bcd
		if ((i - 2) % 24 == 0) {

		}
		// south pole, Tri abd
		else if ((i + 1) % 24 == 0) {

		}
		
		// Tri abd
		else if (i % 2 == 1) {

		}
		// Tri bcd
		else {

		}
		m->GenerateNormals();
		m->GenerateTangents();
		m->BufferData();
		heightMesh.push_back(m);
	}
}

void SphereHeightMap::Draw() {
	for (vector<Mesh*>::const_iterator i = this->GetMeshIteratorStart(); i != this->GetMeshIteratorEnd(); ++i) {
		(*i)->Draw();
	}
}