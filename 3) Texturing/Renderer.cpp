#include "Renderer.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();

	texture = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!texture) {
		return;
	}

	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	if (!shader->LoadSuccess())
	{
		return;
	}
	filtering = true;
	repeating = false;
	init = true;
	i = 0;
}

Renderer::~Renderer(void) {
	delete triangle;
	delete shader;
	glDeleteTextures(1, &texture);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);	//this last parameter
	glActiveTexture(GL_TEXTURE0);		//should match this number!
	glBindTexture(GL_TEXTURE_2D, texture);

	triangle->Draw();

	if (i == 10) {
		GLubyte* bufferData = new GLubyte[512 * 512 * 3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, bufferData);
		for (int j = 0; j < 400; j++) {
			std::cout << Vector3(bufferData[3 * i], bufferData[3 * i + 1], bufferData[3 * i + 2]);
		}
		std::cout << "//////////////////////////////" << std::endl;
		delete[]bufferData;
	}

	if (i == 20) {
		glClear(GL_COLOR_BUFFER_BIT);
		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		GLuint pbo;
		glGenBuffers(1, &pbo);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
		glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 3, nullptr, GL_DYNAMIC_READ);

		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		GLubyte* pixelData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		for (int j = 301; j < 600; j++) {
			std::cout << Vector3(pixelData[3 * i], pixelData[3 * i + 1], pixelData[3 * i + 2]);
		}
		glDeleteBuffers(1, &pbo);
		glDeleteFramebuffers(1, &fbo);
	}
	if (i < 100)i++;
}

void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}

void Renderer::ToggleRepeating() {
	repeating = !repeating;
	SetTextureRepeating(texture, repeating);
}

void Renderer::ToggleFiltering() {
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}