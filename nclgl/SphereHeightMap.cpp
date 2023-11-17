#include "SphereHeightMap.h"

SphereHeightMap::SphereHeightMap() : Sphere(15.0, 30) {
	int theta = 180 / this->degree;
	int phi = 360 / this->degree;
	float hei = this->radius;
	for (int z = 0; z <= phi + 1; z++) {
		for (int x = 0; x <= theta; x++) {
			int offset = z * (theta + 1) + x;
			if (offset % 13 == 0) {
				hei = 1.0;
				Vector3 polorCoord = this->CartesianToPolorCoord(this->vertices[offset]);
				polorCoord.x = hei;
				this->vertices[offset] = this->PolorToCartesianCoord(polorCoord);
				continue;
			}
			switch (offset) {
			case 10:  case 23:  case 36:  case 50:  case 56:  case 63:  case 69:
			case 76:  case 83:  case 89:  case 96:  case 102: case 115: case 123:
			case 128: case 135: case 140: case 154: case 166: case 178: case 190:
			case 207: case 217: case 230: case 244: case 258: case 267: case 271:
			case 280: case 284: case 293: case 298: case 307: case 311: case 324:
			case 326: case 335: hei = 1.1; break;

			case 1:   case 14:  case 28:  case 41:  case 53:  case 67:  case 80:
			case 93:  case 105: case 118: case 131: case 144: case 157: case 170:
			case 183: case 196: case 209: case 222: case 248: case 261: case 274:
			case 287: case 300: case 313: hei = 1.05; break;

			case 158: case 171: case 184: case 301: hei = 1.2; break;

			case 159: case 172: case 185: case 302: hei = 1.15; break;

			case 5:   case 19:  case 30:  case 44:  case 60:  case 73:  case 81:  case 84:  case 94:
			case 97:  case 106: case 119: case 125: case 133: case 138: case 147: case 152: case 160:
			case 174: case 186: case 198: case 210: case 223: case 235: case 249: case 262: case 269:
			case 276: case 282: case 290: case 295: case 304: case 308: case 316: case 330: hei = 1.0; break;

			case 6:   case 20:  case 31:  case 45:  case 61:  case 55:  case 68:  case 74:  case 82:  case 85:
			case 95:  case 98:  case 107: case 120: case 126: case 134: case 139: case 148: case 153: case 161:
			case 175: case 187: case 199: case 211: case 224: case 236: case 250: case 263: case 270: case 277:
			case 283: case 291: case 296: case 305: case 309: case 317: case 331: hei = 0.9; break;

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
		if (i % 24 == 0 || (i + 1) % 24 == 0) continue; // They are repeated.

		/* Tri:  b-a
				 |\|
				 c-d  */
				 // north pole, Tri bcd
		if ((i - 1) % 24 == 0) {
			unsigned int b = 0;
			unsigned int c = 0;
			unsigned int d = 0;
			GetVertexIndicesForTri(i, b, c, d);
			Vector3 B = vertices[b];
			Vector3 C = vertices[c];
			Vector3 D = vertices[d];
			Vector2 coordB = textureCoords[b];
			Vector2 coordC = textureCoords[c];
			Vector2 coordD = textureCoords[d];

			heightMesh.push_back(GenSmallMesh(C, D, B, coordC, coordD, coordC, true));
		}
		// south pole, Tri abd
		else if ((i + 2) % 24 == 0) {
			unsigned int a = 0;
			unsigned int b = 0;
			unsigned int d = 0;
			GetVertexIndicesForTri(i, a, b, d);
			Vector3 A = vertices[a];
			Vector3 B = vertices[b];
			Vector3 D = vertices[d];
			Vector2 coordA = textureCoords[a];
			Vector2 coordB = textureCoords[b];
			Vector2 coordD = textureCoords[d];

			heightMesh.push_back(GenSmallMesh(A, B, D, coordA, coordB, coordD, true));
		}
		// Tri abd
		else if (i % 2 == 0) {
			unsigned int a = 0;
			unsigned int b = 0;
			unsigned int d = 0;
			GetVertexIndicesForTri(i, a, b, d);
			Vector3 A = vertices[a];
			Vector3 B = vertices[b];
			Vector3 D = vertices[d];
			Vector2 coordA = textureCoords[a];
			Vector2 coordB = textureCoords[b];
			Vector2 coordD = textureCoords[d];

			heightMesh.push_back(GenSmallMesh(A, B, D, coordA, coordB, coordD, false));
		}
		// Tri bcd
		else {
			unsigned int b = 0;
			unsigned int c = 0;
			unsigned int d = 0;
			GetVertexIndicesForTri(i, b, c, d);
			Vector3 B = vertices[b];
			Vector3 C = vertices[c];
			Vector3 D = vertices[d];
			Vector2 coordB = textureCoords[b];
			Vector2 coordC = textureCoords[c];
			Vector2 coordD = textureCoords[d];

			heightMesh.push_back(GenSmallMesh(C, D, B, coordC, coordD, coordB, false));
		}
	}
}

Mesh* SphereHeightMap::GenSmallMesh(Vector3 A, Vector3 C, Vector3 B, Vector2 texCoordA, Vector2 texCoordC, Vector2 texCoordB, bool isPole) {
	Mesh* m = new Mesh();
	// In triangle ABC, assume that A is the right angle:
	// B
	// |\
	// A-C
	m->numVertices = 231;  //66
	m->numIndices = 1200; //120

	m->vertices = new Vector3[m->numVertices];
	m->indices = new GLuint[m->numIndices];
	m->textureCoords = new Vector2[m->numVertices];

	Vector3 AC = Vector3(C.x - A.x, C.y - A.y, C.z - A.z);
	Vector3 AB = Vector3(B.x - A.x, B.y - A.y, B.z - A.z);
	Vector3 stepAC = Vector3(0.05 * AC.x, 0.05 * AC.y, 0.05 * AC.z);
	Vector3 stepAB = Vector3(0.05 * AB.x, 0.05 * AB.y, 0.05 * AB.z);

	Vector2 texCoordAC = Vector2(texCoordC.x - texCoordA.x, texCoordC.y - texCoordA.y);
	Vector2 texCoordAB = Vector2(texCoordB.x - texCoordA.x, texCoordB.y - texCoordA.y);
	Vector2 texstepAC = Vector2(0.05 * texCoordAC.x, 0.05 * texCoordAC.y);
	Vector2 texstepAB = Vector2(0.05 * texCoordAB.x, 0.05 * texCoordAB.y);

	int verticesIndex = 0;
	for (int y = 0; y <= 20; y++) {
		for (int x = 0; x <= 20 - y; x++) {
			//vertices[i] = A + x * stepAC + y * stepAB;
			m->vertices[verticesIndex] = Vector3(A.x + x * stepAC.x + y * stepAB.x, A.y + x * stepAC.y + y * stepAB.y, A.z + x * stepAC.z + y * stepAB.z);
			m->textureCoords[verticesIndex] = Vector2(texCoordA.x + x * texstepAC.x + y * texstepAB.x, texCoordA.y + x * texstepAC.y + y * texstepAB.y);
			verticesIndex++;
		}
	}

	verticesIndex = 0;
	int indicesIndex = 0;
	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < 20 - y; x++) {
			m->indices[indicesIndex++] = verticesIndex;
			m->indices[indicesIndex++] = verticesIndex + 1;
			m->indices[indicesIndex++] = verticesIndex + 21 - y;
			verticesIndex++;
		}
		verticesIndex++; // skip the last point in each row
	}

	verticesIndex = 0;
	for (int y = 0; y < 19; y++) {
		verticesIndex++;
		for (int x = 1; x < 20 - y; x++) {
			m->indices[indicesIndex++] = verticesIndex;
			m->indices[indicesIndex++] = verticesIndex + 21 - y;
			m->indices[indicesIndex++] = verticesIndex + 20 - y;
			verticesIndex++;
		}
		verticesIndex++;
	}

	//if (!isPole) {
		//for (int i = 0; i < m->numVertices; i++) {
		//	switch (i)
		//	{
		//	case 31: case 32: case 33: case 35: case 36: case 53: m->vertices[i] = SetRadius(m->vertices[i], -0.025); break;

		//	case 39: case 41: case 42: case 43: case 46: case 49: m->vertices[i] = SetRadius(m->vertices[i], -0.05); break;

		//	case 40: case 47: case 48: m->vertices[i] = SetRadius(m->vertices[i], -0.075);

		//	case 14: case 15: case 16:  case 17: case 25: case 57: case 61: m->vertices[i] = SetRadius(m->vertices[i], 0.025); break;

		//	default:
		//		break;
		//	}
		//}
		//}
		//else {
		//	for (int i = 0; i < m->numVertices; i++) {
		//		switch (i)
		//		{
		//		case 0:  case 1:  case 10: case 11: case 20: case 21: case 29: case 30: case 31: case 32:
		//		case 33: case 35: case 36: case 37: case 51: case 53: case 55: m->vertices[i] = SetRadius(m->vertices[i], -0.0125); break;

		//		case 38: case 39: case 41: case 42: case 43: case 44: case 45: case 46: case 49: case 50: m->vertices[i] = SetRadius(m->vertices[i], -0.025); break;

		//		case 40: case 47: case 48: m->vertices[i] = SetRadius(m->vertices[i], -0.03);

		//		case 3:  case 4:  case 5:  case 6:  case 7:  case 8:  case 14: case 17: case 25: case 57:
		//		case 60: case 62: case 63: case 64: m->vertices[i] = SetRadius(m->vertices[i], 0.012); break;

		//		case 15: case 16: case 61: m->vertices[i] = SetRadius(m->vertices[i], 0.025);
		//		default:
		//			break;
		//		}
		//	}
		//}

	float hei = 0.0;
	for (int i = 0; i < m->numVertices; i++) {
		switch (i)
		{
		case 46: case 62: case 68: case 72: case 80: case 86: case 97: case 103: case 113: case 117: case 128: case 142: hei = 0.02; break;
		case 67: case 82: case 99: case 115: case 88: case 71: hei = 0.05; break;
		case 57: case 75: case 91: case 107: case 121: case 135: case 147: hei = 0.0; break;

		case 166: case 170: case 177: case 187: case 196: case 204: case 211: case 217: hei = -0.01; break;
		case 189: case 205: case 212: hei = -0.03; break;
		case 191: case 207: case 213: hei = -0.01; break;
		case 173: case 183: case 193: case 200: case 208: case 214: case 220: hei = 0.0; break;

		case 0:   case 1:   case 2:   case 3:   case 4:   case 5:   case 6:   case 7:   case 8:   case 9:   case 10:  case 11:
		case 12:  case 13:  case 14:  case 15:  case 16:  case 17:  case 18:  case 19:  case 20:  case 21:  case 40:  case 41:
		case 59:  case 60:  case 77:  case 78:  case 94:  case 95:  case 110: case 111: case 125: case 126: case 139: case 140:
		case 152: case 153: case 164: case 165: case 175: case 176: case 185: case 186: case 195: case 202: case 203: case 209:
		case 210: case 215: case 216: case 221: case 224: case 225: case 227: case 228: case 229: case 230: 
			if (!isPole) {
				m->vertices[i] = ResetRadius(m->vertices[i], -0.0075);
			}
			break;

		default:
			break;
		}
		if (hei == 0.0) {
			continue;
		}
		m->vertices[i] = ResetRadius(m->vertices[i], hei);
	}

	m->GenerateNormals();
	m->GenerateTangents();
	m->BufferData();
	return m;

}

Vector3 SphereHeightMap::ResetRadius(Vector3 CartesianCoord, float dRadius) {
	Vector3 polorCoord = CartesianToPolorCoord(CartesianCoord);
	polorCoord.x += dRadius;
	return PolorToCartesianCoord(polorCoord);
}