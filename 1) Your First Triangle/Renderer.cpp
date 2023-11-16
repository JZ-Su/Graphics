#include "Renderer.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	triangle = Mesh::GenerateCircle(10.0);

	basicShader = new Shader("holaVertex.glsl", "holaFragment.glsl");

	if (!basicShader->LoadSuccess()) {
		return;
	}
	init = true;
}

Renderer::~Renderer(void) {
	delete triangle;
	delete basicShader;
}

void Renderer::RenderScene() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // background color
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	BindShader(basicShader);
	UpdateShaderMatrices();
	triangle->Draw();
}