#include "Renderer.h"
#include "../nclgl/Sphere.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	sunTexture = SOIL_load_OGL_texture(TEXTUREDIR"noise.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTexture = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	moonTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"uni_east.jpg", TEXTUREDIR"uni_west.jpg",
		TEXTUREDIR"uni_top.jpg", TEXTUREDIR"uni_bot.jpg",
		TEXTUREDIR"uni_north.jpg", TEXTUREDIR"uni_south.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	//earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!sunTexture || !earthTexture || !waterTexture || !moonTexture || !cubeMap) {
		return;
	}

	SetTextureRepeating(sunTexture,   true);
	SetTextureRepeating(earthTexture, true);
	SetTextureRepeating(waterTexture, true);
	SetTextureRepeating(moonTexture,  true);

	basicShader   = new Shader("SceneVertex.glsl"    , "SceneFragment.glsl"   );
	skyboxShader  = new Shader("skyboxVertex.glsl"   , "skyboxFragment.glsl"  );
	lightShader   = new Shader("PerPixelVertex.glsl" , "PerPixelFragment.glsl");
	//lightShader   = new Shader("BumpVertex.glsl"     , "BumpFragment.glsl"    );
	if (!basicShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess()) {
		return;
	}

	quad = Mesh::GenerateQuad();
	sphere = new Sphere(15.0, 2);
	earthSurface = Sphere::GenHeightMap();
	waterSurface = Sphere::GenWaterWave(15.0, 2);
	root = new SceneNode();

	sun = new SceneNode(sphere, Vector4(1, 1, 1, 1));
	sun->SetMesh(sphere);
	sun->SetColour(Vector4(1.0, 0.2, 0.0, 1.0));
	sun->SetModelScale(Vector3(200, 200, 200));
	sun->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	sun->SetTexture(sunTexture);
	sun->SetShader(basicShader);
	root->AddChild(sun);

	earth = new SceneNode(earthSurface, Vector4(1, 1, 1, 1));
	earth->SetMesh(earthSurface);
	earth->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	earth->SetModelScale(Vector3(40, 40, 40));
	earth->SetTransform(Matrix4::Translation(Vector3(0, 0, 800)));
	earth->SetTexture(earthTexture);
	earth->SetShader(lightShader);
	sun->AddChild(earth);

	water = new SceneNode(waterSurface, Vector4(1, 1, 1, 1));
	water->SetMesh(waterSurface);
	//water->SetMesh(0);
	water->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	water->SetModelScale(Vector3(42, 42, 42));
	water->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	water->SetTexture(waterTexture);
	water->SetShader(lightShader);
	earth->AddChild(water);

	moonNode = new SceneNode();
	earth->AddChild(moonNode);

	moon = new SceneNode(sphere, Vector4(1, 1, 1, 1));
	moon->SetMesh(sphere);
	moon->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	moon->SetModelScale(Vector3(10, 10, 10));
	moon->SetTransform(Matrix4::Translation(Vector3(0, 0, 100)));
	moon->SetTexture(moonTexture);
	moon->SetShader(lightShader);
	moonNode->AddChild(moon);

	camera = new Camera(-15.0f, -50.0f, Vector3(-1000, 300, 1000));
	light = new Light(Vector3(0, 0, 0), Vector4(1, 1, 1, 1), 1000.0f);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer::~Renderer(void) {
	delete sphere;
	delete earthSurface;
	delete waterSurface;

	delete root;

	delete basicShader;
	delete skyboxShader;
	delete lightShader;

	delete camera;
	delete light;

	glDeleteTextures(1, &sunTexture);
	glDeleteTextures(1, &earthTexture);
	glDeleteTextures(1, &moonTexture);
	glDeleteTextures(1, &waterTexture);
	glDeleteTextures(1, &cubeMap);
	glDeleteTextures(1, &earthBump);
}

void Renderer::UpdateScene(float deltaTime, float totalTime) {
	camera->UpdateCamera(deltaTime);
	viewMatrix = camera->BuildViewMatrix();

	waterSurface->Update(totalTime);

	sun->     SetTransform(sun->GetTransform()      * Matrix4::Rotation(-1.0f * deltaTime,  Vector3(0, 1, 0)));
	earth->   SetTransform(earth->GetTransform()    * Matrix4::Rotation(-7.5f * deltaTime,  Vector3(0, 1, 0)));
	water->   SetTransform(water->GetTransform()    * Matrix4::Rotation(-4.5f * deltaTime,  Vector3(0, 1, 0)));
	moonNode->SetTransform(moonNode->GetTransform() * Matrix4::Rotation(-20.0f * deltaTime, Vector3(0, 1, 0)));
	moon->    SetTransform(moon->GetTransform()     * Matrix4::Rotation(-45.0f * deltaTime, Vector3(0, 1, 0)));

	root->Update(deltaTime);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawSkyBox();
	DrawNode(root);
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
		if (n->GetShader() == basicShader) {
			BindShader(basicShader);
			UpdateShaderMatrices();
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(basicShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());
			
			glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "useTexture"), n->GetTexture());

			n->Draw(*this);
		}
		else if (n->GetShader() == lightShader)
		{
			BindShader(lightShader);
			SetShaderLight(*light);
			UpdateShaderMatrices();

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			
			glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());

			/*glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, earthBump);*/

			glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

			n->Draw(*this);
		}
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}
