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
	void InitScene();
	void InitShadow();
	void DrawShadowScene();

	Camera* camera;
	float   cameraSpeed;
	Vector3 autoCameraPosition;
	float   autoCameraYaw;
	float   autoCameraPitch;
	float   autoCameraRoll;
	float   fov;

	SceneNode* root;
	SceneNode* sun;
	SceneNode* earth;
	SceneNode* sky;
	SceneNode* water;
	SceneNode* moonNode;
	SceneNode* moon;
	SceneNode* HALO;

	Sphere*	sphere;
	SphereHeightMap* earthSurface;
	Sphere* waterSurface;

	GLuint  cubeMap;
	Shader* skyboxShader;
	Shader* basicShader;
	Shader* holaShader;

	Light*  light;
	Shader* lightShader;
	GLuint shadowTex;
	GLuint shadowFBO;
	Shader* shadowShader;

	GLuint  sunTexture;
	GLuint  earthTexture;
	GLuint  waterTexture;
	GLuint  earthBump;
	GLuint  skyTexture;

	Mesh*   quad;
	Mesh*   circle;
};
