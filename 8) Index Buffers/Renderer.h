#pragma once
#include "../nclgl/OGLRenderer.h"
class HeightMap;
class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;
	void SwitchPolygenMode();

protected:
	HeightMap* heightMap;
	Shader* shader;
	Camera* camera;
	GLuint terrainTex;
	Mesh* axis[3];
	bool showMesh;
};

