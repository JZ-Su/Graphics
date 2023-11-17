#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"
#include "../nclgl/Sphere.h"
#include "../nclgl/SphereHeightMap.h"

#define SHADOWSIZE 2048

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	sunTexture = SOIL_load_OGL_texture(TEXTUREDIR"noise.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTexture = SOIL_load_OGL_texture(TEXTUREDIR"reflection.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	moonTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"uni_east.jpg", TEXTUREDIR"uni_west.jpg",
		TEXTUREDIR"uni_top.jpg", TEXTUREDIR"uni_bot.jpg",
		TEXTUREDIR"uni_north.jpg", TEXTUREDIR"uni_south.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!sunTexture || !earthTexture || !waterTexture || !moonTexture || !cubeMap || !earthBump) {
		return;
	}

	SetTextureRepeating(sunTexture, true);
	SetTextureRepeating(earthTexture, true);
	SetTextureRepeating(waterTexture, true);
	SetTextureRepeating(moonTexture, true);
	SetTextureRepeating(earthBump, true);

	basicShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	holaShader = new Shader("holaVertex.glsl", "holaFragment.glsl");
	shadowShader = new Shader("shadowSceneVert.glsl", "shadowSceneFrag.glsl");
	//lightShader   = new Shader("BumpVertex.glsl"     , "BumpFragment.glsl"    );
	if (!basicShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess() || !holaShader->LoadSuccess()) {
		return;
	}

	autoCameraPosition = Vector3(0, 50, 850);
	autoCameraYaw = 0.0f;
	autoCameraPitch = -15.0f;
	autoCameraRoll = 0.0f;
	camera = new Camera(autoCameraPitch, autoCameraYaw, autoCameraRoll, autoCameraPosition);
	cameraSpeed = 100.0f;
	freeCamera = true;

	InitScene();
	InitShadow();

	fov = 20.0f;
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, fov);

	light = new Light(Vector3(0, 0, 0), Vector4(1, 1, 1, 1), 1000.0f);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
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

void Renderer::InitScene() {
	quad = Mesh::GenerateQuad();
	circle = Mesh::GenerateCircle(10.0);
	sphere = new Sphere(15.0, 2);
	//earthSurface = Sphere::GenHeightMap();
	earthSurface = new SphereHeightMap();
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
	earth->SetMesh(0);
	earth->SetColour(Vector4(1.0, 1.0, 1.0, 0.0));
	earth->SetModelScale(Vector3(40, 40, 40));
	earth->SetTransform(Matrix4::Translation(Vector3(0, 0, 800)));
	earth->SetTexture(earthTexture);
	earth->SetShader(shadowShader);
	sun->AddChild(earth);

	earth->SetPreviousPosition(earth->GetTransform().GetPositionVector());

	for (int i = 0; i < earthSurface->GetMesh().size(); i++) {
		SceneNode* s = new SceneNode();
		s->SetMesh(earthSurface->GetMesh()[i]);
		s->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
		s->SetModelScale(Vector3(40, 40, 40));
		s->SetShader(shadowShader);
		s->SetTexture(earthTexture);
		s->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
		earth->AddChild(s);
	}

	water = new SceneNode(waterSurface, Vector4(1, 1, 1, 1));
	water->SetMesh(waterSurface);
	water->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	water->SetModelScale(Vector3(41, 41, 41));
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
	moon->SetShader(shadowShader);
	moonNode->AddChild(moon);

	HOLA = new SceneNode();
	root->AddChild(HOLA);

	for (int i = 0; i < 4; i++) {
		SceneNode* hola = new SceneNode();
		hola->SetMesh(circle);
		hola->SetColour(Vector4(1, 1, 1, 0.1));
		hola->SetTransform(Matrix4::Translation(autoCameraPosition * 0.3 * i));
		hola->SetModelScale(Vector3(217 - 138 * i + 23 * i * i, 217 - 138 * i + 23 * i * i, 217 - 138 * i + 23 * i * i));
		//hola->SetModelScale(Vector3(7 * (4 - i), 7 * (4 - i), 7 * (4 - i)));
		hola->SetShader(holaShader);
		HOLA->AddChild(hola);
	}
}

void Renderer::InitShadow() {
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::UpdateProjMatrix(float df) {
	fov -= df;
	fov = std::max(1.0f, fov);
	fov = std::min(90.0f, fov);
	//projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, fov);
}

void Renderer::UpdateScene(float deltaTime, float totalTime) {
	if (freeCamera) {
		if ((camera->GetPosition() - earth->GetWorldTransform().GetPositionVector()).Length() < 60) {
			//camera->AutoCamera(Matrix4::Rotation(-PI / 360.0 * totalTime, Vector3(0, 1, 0)) * camera->GetPrePosition(),
			//	camera->GetPitch(), camera->GetYaw(), camera->GetRoll());
			camera->SetCamera(camera->GetPosition() + earth->GetWorldTransform().GetPositionVector() - earth->GetPreviousPosition(),
				camera->GetPitch(), camera->GetYaw(), camera->GetRoll());
			cameraSpeed = 10.0f;
		}
		else {
			cameraSpeed = 100.0f;
		}
		camera->UpdateCamera(deltaTime, cameraSpeed);
	}
	else {
		camera->SetCamera(autoCameraPosition, autoCameraPitch, autoCameraYaw, autoCameraRoll);
	}
	//viewMatrix = camera->BuildViewMatrix();
	waterSurface->Update(totalTime);

	sun->SetTransform(sun->GetTransform() * Matrix4::Rotation(-1.0f * deltaTime, Vector3(0, 1, 0)));
	earth->SetPreviousPosition(earth->GetWorldTransform().GetPositionVector());
	earth->SetTransform(earth->GetTransform() * Matrix4::Rotation(-7.5f * deltaTime, Vector3(0, 1, 0)));
	water->SetTransform(water->GetTransform() * Matrix4::Rotation(-4.5f * deltaTime, Vector3(0, 1, 0)));
	moonNode->SetTransform(moonNode->GetTransform() * Matrix4::Rotation(-20.0f * deltaTime, Vector3(0, 1, 0)));
	moon->SetTransform(moon->GetTransform() * Matrix4::Rotation(-45.0f * deltaTime, Vector3(0, 1, 0)));

	autoCameraPosition = earth->GetWorldTransform().GetPositionVector() + Vector3(0, 43, 0);;
	//autoCameraPosition = Vector3(-800 * sin(6.285 / 360.0 * totalTime), 43, 800 * cos(6.285 / 360.0 * totalTime));
	autoCameraYaw -= deltaTime;
	if (autoCameraYaw <= 0.0f) {
		autoCameraYaw += 360.0f;
	}

	Vector3 prePosition = camera->GetPrePosition();
	Vector3 nowPosition = camera->GetPosition();

	float prePitch = RadToDeg(asin(prePosition.y / prePosition.Length()));
	float preYaw = RadToDeg(atan2(prePosition.z, prePosition.x));
	float nowPitch = RadToDeg(asin(nowPosition.y / nowPosition.Length()));
	float nowYaw = RadToDeg(atan2(nowPosition.z, nowPosition.x));

	float deltaPitch = nowPitch - prePitch;
	float deltaYaw = nowYaw - preYaw;
	if (deltaPitch || deltaYaw) {
		HOLA->SetTransform(HOLA->GetTransform() * Matrix4::Rotation(-deltaPitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-deltaYaw, Vector3(0, 1, 0)));
	}
	//if (deltaPosition.Length()) {
	//	float p = RadToDeg(asin(deltaPosition.y / sqrt(deltaPosition.Length())));
	//	float y = RadToDeg(atan2(deltaPosition.x, deltaPosition.z));
	//	float r = RadToDeg(atan2(deltaPosition.y, deltaPosition.x));
	//	HOLA->SetTransform(HOLA->GetTransform() * Matrix4::Rotation(-y / 360, Vector3(1, 0, 0)) * Matrix4::Rotation(-p / 360, Vector3(0, 1, 0))
	//		 * Matrix4::Rotation(-r / 360, Vector3(0, 0, 1)));
	//}
	HOLA->SetTransform(HOLA->GetTransform() * Matrix4::Rotation(-camera->GetDeltaPitch(), Vector3(1, 0, 0)) * Matrix4::Rotation(-camera->GetDeltaYaw(), Vector3(0, 1, 0)));

	root->Update(deltaTime);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	DrawShadowScene();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, fov);
	DrawSkyBox();
	DrawNode(root);

	glViewport(0, 0, width / 3, height / 3);
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawSkyBox();
	DrawNode(root);
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//BindShader(shadowShader);
	BindShader(basicShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	shadowMatrix = Matrix4::Perspective(1, 100, 1, 45);

	DrawNode(root);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		else if (n->GetShader() == lightShader) {
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
		else if (n->GetShader() == shadowShader) {
			BindShader(shadowShader);
			SetShaderLight(*light);
			UpdateShaderMatrices();

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(), "modelMatrix"), 1, false, model.values);

			glUniform1i(glGetUniformLocation(shadowShader->GetProgram(), "diffuseTex"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());

			glUniform1i(glGetUniformLocation(shadowShader->GetProgram(), "bumpTex"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, earthBump);

			glUniform3fv(glGetUniformLocation(shadowShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

			n->Draw(*this);
		}
		else if (n->GetShader() == holaShader) {
			BindShader(holaShader);
			UpdateShaderMatrices();
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(holaShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(holaShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			n->Draw(*this);
		}
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}

void Renderer::renderscene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawSkyBox();
	DrawNode(root);
}