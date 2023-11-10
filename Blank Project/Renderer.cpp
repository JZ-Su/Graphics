#include "Renderer.h"
#include "../nclgl/Camera.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	//triangle = Mesh::GenerateTriangle();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");  //numVertices = 515

	basicShader = new Shader("SceneVertex.glsl","SceneFragment.glsl");
	camera = new Camera();

	if(!basicShader->LoadSuccess()) {
		return;
	}

	texture = SOIL_load_OGL_texture(TEXTUREDIR"noise.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!texture) {
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	camera->SetPosition(Vector3(0, 0, 10));
	glEnable(GL_DEPTH_TEST);
	init = true;
}
Renderer::~Renderer(void)	{
	delete sphere;
	delete basicShader;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	BindShader(basicShader);
	UpdateShaderMatrices();
	modelMatrix = Matrix4::Translation(Vector3(0, 0, 0)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(0, Vector3(1, 1, 1));
	Vector4 nodeColour = Vector4(1.0f, 0.1f, 0.0f, 1.0f);

	//triangle->Draw();
	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "useTexture"), 1);
	glUniform4fv(glGetUniformLocation(basicShader->GetProgram(), "nodeColour"), 1, (float*)&nodeColour);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	sphere->Draw();
}

