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
	skyTexture = SOIL_load_OGL_texture(TEXTUREDIR"cloud.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTexture = SOIL_load_OGL_texture(TEXTUREDIR"reflection.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	haloTexture = SOIL_load_OGL_texture(TEXTUREDIR"light-effect.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	treeTex = SOIL_load_OGL_texture(TEXTUREDIR"Tree.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	treeBumpTex = SOIL_load_OGL_texture(TEXTUREDIR"Tree_N.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"uni_east.jpg", TEXTUREDIR"uni_west.jpg",
		TEXTUREDIR"uni_top.jpg", TEXTUREDIR"uni_bot.jpg",
		TEXTUREDIR"uni_north.jpg", TEXTUREDIR"uni_south.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!sunTexture || !earthTexture || !skyTexture || !waterTexture || !haloTexture || !bumpTex || !treeTex || !treeBumpTex || !cubeMap) {
		return;
	}

	SetTextureRepeating(sunTexture, true);
	SetTextureRepeating(earthTexture, true);
	SetTextureRepeating(skyTexture, true);
	SetTextureRepeating(waterTexture, true);
	SetTextureRepeating(haloTexture, true);
	SetTextureRepeating(bumpTex, true);
	SetTextureRepeating(treeTex, true);

	basicShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	haloShader = new Shader("haloVertex.glsl", "haloFragment.glsl");
	shadowShader = new Shader("shadowSceneVert.glsl", "shadowSceneFrag.glsl");
	if (!basicShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !haloShader->LoadSuccess() || !shadowShader->LoadSuccess()) {
		return;
	}

	autoCameraPosition = Vector3(100, 150, 1200);
	autoCameraPitch = -15.0f;
	autoCameraYaw   = 15.0f;
	autoCameraRoll  = 0.0f;
	camera = new Camera(autoCameraPitch, autoCameraYaw, autoCameraRoll, autoCameraPosition);
	cameraSpeed = 100.0f;
	freeCamera = true;

	fov = 30.0f;
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, fov);

	light = new Light(Vector3(0, 0, 0), Vector4(1, 1, 1, 1), 1000.0f);

	InitScene();
	InitShadow();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer::~Renderer(void) {
	delete camera;
	delete root;

	delete quad;
	delete circle;
	delete sphere;
	delete waterSurface;
	delete earthSurface;

	delete basicShader;
	delete skyboxShader;
	delete shadowShader;
	delete haloShader;

	delete light;

	glDeleteTextures(1, &sunTexture);
	glDeleteTextures(1, &earthTexture);
	glDeleteTextures(1, &skyTexture);
	glDeleteTextures(1, &waterTexture);
	glDeleteTextures(1, &haloTexture);
	glDeleteTextures(1, &bumpTex);
	glDeleteTextures(1, &treeTex);
	glDeleteTextures(1, &treeBumpTex);
	glDeleteTextures(1, &cubeMap);
}

void Renderer::InitScene() {
	quad = Mesh::GenerateQuad();
	circle = Mesh::GenerateCircle(10.0);
	sphere = new Sphere(15.0, 2);
	earthSurface = new SphereHeightMap();
	waterSurface = Sphere::GenWaterWave(15.0, 2);
	treeMesh = Mesh::LoadFromMeshFile("Tree.msh");

	root = new SceneNode();
	root->SetTransform(Matrix4());

	sun = new SceneNode();
	sun->SetMesh(sphere);
	sun->SetColour(Vector4(1.0, 0.2, 0.0, 1.0));
	sun->SetModelScale(Vector3(200, 200, 200));
	sun->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	sun->SetTexture(sunTexture);
	sun->SetBumpTex(0);
	sun->SetShader(basicShader);
	root->AddChild(sun);

	earth = new SceneNode();
	earth->SetTransform(Matrix4::Translation(Vector3(0, 0, 800)));
	sun->AddChild(earth);

	earth->SetPreviousPosition(earth->GetTransform().GetPositionVector());

	for (int i = 0; i < earthSurface->GetMesh().size(); i++) {
		SceneNode* s = new SceneNode();
		s->SetMesh(earthSurface->GetMesh()[i]);
		s->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
		s->SetModelScale(Vector3(40, 40, 40));
		s->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
		s->SetTexture(earthTexture);
		s->SetBumpTex(bumpTex);
		s->SetShader(shadowShader);
		earth->AddChild(s);
	}

	sky = new SceneNode();
	sky->SetMesh(sphere);
	sky->SetColour(Vector4(1, 1, 1, 0.1));
	sky->SetModelScale(Vector3(80, 80, 80));
	sky->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	sky->SetTexture(skyTexture);
	sky->SetBumpTex(0);
	sky->SetShader(shadowShader);
	earth->AddChild(sky);

	water = new SceneNode();
	water->SetMesh(waterSurface);
	water->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	water->SetModelScale(Vector3(41, 41, 41));
	water->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	water->SetTexture(waterTexture);
	water->SetBumpTex(bumpTex);
	water->SetShader(shadowShader);
	earth->AddChild(water);

	moonNode = new SceneNode();
	earth->AddChild(moonNode);

	moon = new SceneNode();
	moon->SetMesh(sphere);
	moon->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	moon->SetModelScale(Vector3(10, 10, 10));
	moon->SetTransform(Matrix4::Translation(Vector3(0, 0, 100)));
	moon->SetTexture(earthTexture);
	moon->SetBumpTex(bumpTex);
	moon->SetShader(shadowShader);
	moonNode->AddChild(moon);

	treeNode = new SceneNode();
	earth->AddChild(treeNode);	
	Matrix4 treeTranslation[6] = { Matrix4::Translation(Vector3(-10, 41, 0)) * Matrix4::Rotation(30, Vector3(0, 1, 1)),
		Matrix4::Translation(Vector3(-19.5,   43, -10.5)) * Matrix4::Rotation( 10, Vector3(0, 1, 1)),
		Matrix4::Translation(Vector3(-21.5,   41, -12.5)) * Matrix4::Rotation( 30, Vector3(0, 0, 1)),
		Matrix4::Translation(Vector3(-17  ,   40,     0)) * Matrix4::Rotation( 15, Vector3(1, 0, 0)),
		Matrix4::Translation(Vector3( 20  , 40.5,    -5)) * Matrix4::Rotation( 30, Vector3(0, 1, 1)),
		Matrix4::Translation(Vector3( 16  , 35.5,    16)) * Matrix4::Rotation(-15, Vector3(1, 0, 0)) };

	for (int i = 0; i < 6; i++) {
		SceneNode* tree = new SceneNode();
		tree->SetMesh(treeMesh);
		tree->SetColour(Vector4(1, 1, 1, 1));
		tree->SetTransform(treeTranslation[i]);
		tree->SetModelScale(Vector3(0.1, 0.1, 0.1));
		tree->SetTexture(treeTex);
		tree->SetBumpTex(treeBumpTex);
		tree->SetShader(shadowShader);
		treeNode->AddChild(tree);
	}

	haloNode = new SceneNode();
	root->AddChild(haloNode);
	for (int i = 0; i < 5; i++) {
		halo[i] = new SceneNode();
		halo[i]->SetMesh(circle);
		halo[i]->SetColour(Vector4(1, 1, 1, 0.05));
		halo[i]->SetModelScale(Vector3(8.75 * i * i - 70.0 * i + 180, 8.75 * i * i - 70.0 * i + 180, 8.75 * i * i - 70.0 * i + 180));
		halo[i]->SetTransform(Matrix4::Translation(Vector3(0, 0, 210 + i * 120)));
		halo[i]->SetTexture(haloTexture);
		halo[i]->SetShader(haloShader);
		haloNode->AddChild(halo[i]);
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

void Renderer::UpdateProjMatrixFov(float df) {
	fov -= df;
	fov = std::max(1.0f, fov);
	fov = std::min(90.0f, fov);
}

void Renderer::UpdateScene(float deltaTime, float totalTime) {
	if (freeCamera) {
		// if camera gets close enough to the earth, attach to it and slow down
		if ((camera->GetPosition() - earth->GetWorldTransform().GetPositionVector()).Length() < 80) {
			//camera->SetCamera(Matrix4::Rotation(-PI / 360.0 * totalTime, Vector3(0, 1, 0)) * camera->GetPrePosition(), camera->GetPitch(), camera->GetYaw(), camera->GetRoll());
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

	// Update water wave
	waterSurface->Update(totalTime);

	sun->SetTransform(sun->GetTransform() * Matrix4::Rotation(-1.0f * deltaTime, Vector3(0, 1, 0)));
	earth->SetPreviousPosition(earth->GetWorldTransform().GetPositionVector());
	earth->SetTransform(earth->GetTransform() * Matrix4::Rotation(-7.5f * deltaTime, Vector3(0, 1, 0)));
	sky->SetTransform(sky->GetTransform() * Matrix4::Rotation(-7.5f * deltaTime, Vector3(0, 1, 0)));
	water->SetTransform(water->GetTransform() * Matrix4::Rotation(-4.5f * deltaTime, Vector3(0, 1, 0)));
	moonNode->SetTransform(moonNode->GetTransform() * Matrix4::Rotation(-20.0f * deltaTime, Vector3(0, 1, 0)));
	moon->SetTransform(moon->GetTransform() * Matrix4::Rotation(-45.0f * deltaTime, Vector3(0, 1, 0)));

	// Update auto camera position
	//autoCameraPosition = earth->GetWorldTransform().GetPositionVector() + Vector3(0, 75, 200);;
	autoCameraPosition = Vector3(100 - 1000 * sin(PI / 180.0 * totalTime), 150, 1000 * cos(PI / 180.0 * totalTime));
	autoCameraYaw -= deltaTime;
	if (autoCameraYaw <= 0.0f) {
		autoCameraYaw += 360.0f;
	}

	// Set halo trnasformation, when using keyboard
	Vector3 prePosition = camera->GetPrePosition();
	Vector3 nowPosition = camera->GetPosition();
	float prePitch = RadToDeg(asin(prePosition.y / prePosition.Length()));
	float preYaw = RadToDeg(atan2(prePosition.z, prePosition.x));
	float nowPitch = RadToDeg(asin(nowPosition.y / nowPosition.Length()));
	float nowYaw = RadToDeg(atan2(nowPosition.z, nowPosition.x));
	float deltaPitch = nowPitch - prePitch;
	float deltaYaw = nowYaw - preYaw;
	if (deltaPitch || deltaYaw) {
		haloNode->SetTransform(haloNode->GetTransform() * Matrix4::Rotation(-deltaPitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-deltaYaw, Vector3(0, 1, 0)));
	}
	// when using mouse
	haloNode->SetTransform(haloNode->GetTransform() * Matrix4::Rotation(-camera->GetDeltaPitch(), Vector3(1, 0, 0)) * Matrix4::Rotation(-camera->GetDeltaYaw(), Vector3(0, 1, 0)));
	for (int i = 0; i < 5; i++) {
		halo[i]->SetTransform(halo[i]->GetTransform() * Matrix4::Rotation(camera->GetDeltaPitch() * PI / 2, Vector3(1, 0, 0)) * Matrix4::Rotation(camera->GetDeltaYaw() * PI / 2, Vector3(0, 1, 0)));
	}

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

			glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "useTexture"), n->GetTexture());
			if (n->GetTexture()) {
				glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "diffuseTex"), 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, n->GetTexture());
			}
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

			if (n->GetBumpTex()) {
				glUniform1i(glGetUniformLocation(shadowShader->GetProgram(), "useBumpTex"), true);
				glUniform1i(glGetUniformLocation(shadowShader->GetProgram(), "bumpTex"), 1);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, n->GetBumpTex());
			}
			else {
				glUniform1i(glGetUniformLocation(shadowShader->GetProgram(), "useBumpTex"), false);
			}

			glUniform1i(glGetUniformLocation(shadowShader->GetProgram(), "shadowTex"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, shadowTex);

			glUniform3fv(glGetUniformLocation(shadowShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

			n->Draw(*this);
		}
		else if (n->GetShader() == haloShader) {
			BindShader(haloShader);
			UpdateShaderMatrices();
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(haloShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(haloShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glUniform1i(glGetUniformLocation(haloShader->GetProgram(), "diffuseTex"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->GetTexture());

			n->Draw(*this);
		}
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	BindShader(basicShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	shadowMatrix = Matrix4::Perspective(1, 10000, 1, 45);

	DrawShadowNode(root);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadowNode(SceneNode* n) {
	if (n->GetMesh()) {
		UpdateShaderMatrices();
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		n->Draw(*this);
	}
	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawShadowNode(*i);
	}
}