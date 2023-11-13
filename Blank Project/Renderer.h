#pragma once
#include "../NCLGL/OGLRenderer.h"
class Camera;
class Sphere;
class SceneNode;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float deltaTime, float totalTime);

	 void DrawNode(SceneNode* n);
	 void DrawSkyBox();
protected:
	SceneNode* root;
	SceneNode* sun;
	SceneNode* earth;
	SceneNode* water;
	SceneNode* moonNode;
	SceneNode* moon;

	Sphere*	sphere;
	Sphere* earthSurface;
	Sphere* skySurface;
	Sphere* waterSurface;

	Shader* basicShader;
	Camera* camera;

	GLuint  sunTexture;
	GLuint  earthTexture;
	GLuint  waterTexture;
	GLuint  moonTexture;

	GLuint  cubeMap;
	Shader* skyboxShader;

	Mesh*	axis[3];
	Mesh*   quad;
};
