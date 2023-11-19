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
	 void DrawShadowNode(SceneNode* n);
	 void DrawSkyBox();

	 void UpdateProjMatrixFov(float wheelMovement);

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
	SceneNode* haloNode;
	SceneNode* halo[5];

	Mesh*   quad;
	Mesh*   circle;
	Sphere*	sphere;
	Sphere* waterSurface;
	SphereHeightMap* earthSurface;

	Shader* basicShader;
	Shader* skyboxShader;
	Shader* shadowShader;
	Shader* haloShader;

	Light*  light;
	GLuint  shadowTex;
	GLuint  shadowFBO;

	GLuint  sunTexture;
	GLuint  earthTexture;
	GLuint  skyTexture;
	GLuint  waterTexture;
	GLuint  haloTexture;
	GLuint  bumpTex;
	GLuint  cubeMap;
};
