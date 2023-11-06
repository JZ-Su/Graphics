#pragma once
#include "..\nclgl\OGLRenderer.h"
class Camera;
class Mesh;
class MeshAnimation;
class MeshMaterial;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	Camera* camera;
	Mesh* mesh;
	Shader* shader;
	MeshAnimation* anim;
	MeshMaterial* material;
	vector<GLuint> matTexture;

	int currentFrame;
	float frameTime;
};

