#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Make your own project!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}
	bool showLine = false;
	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F)) {
			showLine = !showLine;
			if (showLine) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds(), w.GetTimer()->GetTotalTimeSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}
	return 0;
}