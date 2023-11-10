#pragma once
#include "../NCLGL/OGLRenderer.h"
class Camera;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float dt)		override;
protected:
	Mesh*	triangle;
	Mesh*	sphere;
	Shader* basicShader;
	Camera* camera;
	GLuint  texture;
};
