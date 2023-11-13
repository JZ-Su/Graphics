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
	Camera* camera;

	SceneNode* root;
	SceneNode* sun;
	SceneNode* earth;
	SceneNode* waterNode;
	SceneNode* water;
	SceneNode* moonNode;
	SceneNode* moon;

	Sphere*	sphere;
	Sphere* earthSurface;
	Sphere* skySurface;
	Sphere* waterSurface;

	GLuint  cubeMap;
	Shader* skyboxShader;
	Shader* basicShader;

	Light*  light;
	Shader* lightShader;

	GLuint  sunTexture;
	GLuint  earthTexture;
	GLuint  waterTexture;
	GLuint  moonTexture;
	GLuint  earthBump;

	Mesh*   quad;
};
