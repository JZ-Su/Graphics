#pragma once
#include "../NCLGL/OGLRenderer.h"
class Camera;
class Sphere;
class SphereHeightMap;
class SceneNode;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float deltaTime, float totalTime);

	 void DrawNode(SceneNode* n);
	 void DrawSkyBox();

	 void UpdateProjMatrix(float wheelMovement);

	 bool freeCamera;

protected:
	Camera* camera;
	Vector3 cameraPosition;
	float   cameraYaw;
	float   cameraPitch;
	float   fov;

	SceneNode* root;
	SceneNode* sun;
	SceneNode* earth;
	SceneNode* water;
	SceneNode* moonNode;
	SceneNode* moon;

	Sphere*	sphere;
	SphereHeightMap* earthSurface;
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
