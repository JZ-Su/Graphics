#include "Renderer.h"
#include "../nclgl/Sphere.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	sunTexture = SOIL_load_OGL_texture(TEXTUREDIR"noise.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTexture = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	moonTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"GalaxyLeft.jpg", TEXTUREDIR"GalaxyRight.jpg",
	//	TEXTUREDIR"GalaxyTop.jpg", TEXTUREDIR"GalaxyBottom.jpg",
	//	TEXTUREDIR"GalaxyBack", TEXTUREDIR"GalaxyFront.jpg",
	//	SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!sunTexture || !earthTexture || !waterTexture || !moonTexture) {
		return;
	}

	SetTextureRepeating(sunTexture, true);
	SetTextureRepeating(earthTexture, true);
	SetTextureRepeating(waterTexture, true);
	SetTextureRepeating(moonTexture, true);

	quad = Mesh::GenerateQuad();

	sphere = new Sphere(15.0, 2, false);
	earthSurface = new Sphere(15.0, 8, false);
	earthSurface->GenHeightMap();
	skySurface = new Sphere(15.0, 8, true);
	waterSurface = new Sphere(15.0, 6, false);
	waterSurface->GenWaterWave();
	root = new SceneNode();

	sun = new SceneNode(sphere, Vector4(1, 1, 1, 1));
	sun->SetMesh(sphere);
	sun->SetColour(Vector4(1.0, 0.2, 0.0, 1.0));
	sun->SetModelScale(Vector3(150, 150, 150));
	sun->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	sun->SetTexture(sunTexture);
	root->AddChild(sun);

	earth = new SceneNode(earthSurface, Vector4(1, 1, 1, 1));
	earth->SetMesh(earthSurface);
	earth->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	earth->SetModelScale(Vector3(40, 40, 40));
	earth->SetTransform(Matrix4::Translation(Vector3(0, 0, 800)));
	earth->SetTexture(earthTexture);
	sun->AddChild(earth);

	water = new SceneNode(waterSurface, Vector4(1, 1, 1, 1));
	water->SetMesh(waterSurface);
	water->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	water->SetModelScale(Vector3(40.1, 40.1, 40.1));
	water->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	water->SetTexture(waterTexture);
	earth->AddChild(water);

	moonNode = new SceneNode();
	earth->AddChild(moonNode);

	moon = new SceneNode(sphere, Vector4(1, 1, 1, 1));
	moon->SetMesh(sphere);
	moon->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	moon->SetModelScale(Vector3(10, 10, 10));
	moon->SetTransform(Matrix4::Translation(Vector3(0, 0, 100)));
	moon->SetTexture(moonTexture);
	moonNode->AddChild(moon);

	axis[0] = Mesh::GenerateXAxis();
	axis[1] = Mesh::GenerateYAxis();
	axis[2] = Mesh::GenerateZAxis();

	basicShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	if (!basicShader->LoadSuccess()) {
		return;
	}
	//skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	//if (!skyboxShader->LoadSuccess()) {
	//	return;
	//}

	camera = new Camera(-30.0f, 30.0f, Vector3(300, 300, 500));
	//camera = new Camera(-90.0f, 0.0f, Vector3(0, 60, 0));
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	init = true;
}

Renderer::~Renderer(void) {
	delete sphere;
	delete basicShader;
	glDeleteTextures(1, &sunTexture);
	glDeleteTextures(1, &earthTexture);
	glDeleteTextures(1, &moonTexture);
	glDeleteTextures(1, &waterTexture);
	delete root;
}

void Renderer::UpdateScene(float deltaTime, float totalTime) {
	//std::cout << totalTime << "\n";
	camera->UpdateCamera(deltaTime);
	viewMatrix = camera->BuildViewMatrix();

	waterSurface->Update(totalTime);

	sun->SetTransform(sun->GetTransform() * Matrix4::Rotation(-3.0f * deltaTime, Vector3(0, 1, 0)));
	earth->SetTransform(earth->GetTransform() * Matrix4::Rotation(-4.5f * deltaTime, Vector3(0, 1, 0)));
	moonNode->SetTransform(moonNode->GetTransform() * Matrix4::Rotation(-20.0f * deltaTime, Vector3(0, 1, 0)));
	moon->SetTransform(moon->GetTransform() * Matrix4::Rotation(-90.0f * deltaTime, Vector3(0, 1, 0)));

	root->Update(deltaTime);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(basicShader);
	UpdateShaderMatrices();
	for (int i = 0; i < 3; i++) {
		axis[i]->Draw();
	}

	//modelMatrix = Matrix4::Translation(Vector3(0, 0, 0)) * Matrix4::Scale(Vector3(1, 1, 1)) * Matrix4::Rotation(0, Vector3(0, 0, 0));
	//Vector4 nodeColour = Vector4(1.0f, 0.2f, 0.0f, 1.0f);

	//glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "diffuseTex"), 0);
	//glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "useTexture"), 1);
	//glUniform4fv(glGetUniformLocation(basicShader->GetProgram(), "nodeColour"), 1, (float*)&nodeColour);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, sunTexture);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawNode(root);
	//DrawSkyBox();
}

void Renderer::DrawSkyBox() {
	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(basicShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		GLuint texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "useTexture"), texture);

		n->Draw(*this);
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}
