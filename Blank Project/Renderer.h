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
	 void renderscene();
	 void UpdateScene(float deltaTime, float totalTime);

	 void DrawNode(SceneNode* n);
	 void DrawSkyBox();

	 void UpdateProjMatrix(float wheelMovement);

	 bool freeCamera;

protected:
	Camera* camera;
	Vector3 autoCameraPosition;
	float   autoCameraYaw;
	float   autoCameraPitch;
	float   autoCameraRoll;
	float   fov;

	SceneNode* root;
	SceneNode* sun;
	SceneNode* earth;
	SceneNode* water;
	SceneNode* moonNode;
	SceneNode* moon;
	SceneNode* HOLA;

	Sphere*	sphere;
	SphereHeightMap* earthSurface;
	Sphere* waterSurface;

	GLuint  cubeMap;
	Shader* skyboxShader;
	Shader* basicShader;
	Shader* holaShader;

	Light*  light;
	Shader* lightShader;

	GLuint  sunTexture;
	GLuint  earthTexture;
	GLuint  waterTexture;
	GLuint  moonTexture;
	GLuint  earthBump;

	Mesh*   quad;
	Mesh*   hexagon;
};
