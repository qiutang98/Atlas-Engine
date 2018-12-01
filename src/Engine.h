#ifndef ENGINE_H
#define ENGINE_H

#include "System.h"
#include "Window.h"
#include "Camera.h"
#include "Scene.h"
#include "RenderTarget.h"
#include "shader/Shader.h"
#include "renderer/MasterRenderer.h"
#include "events/SystemEventHandler.h"

class Engine {

public:
	static Window* Init(string shaderDirectory, string windowTitle, int32_t x, int32_t y,
			int32_t width, int32_t height, int32_t flags = WINDOW_FULLSCREEN);

	///
	static void LockFramerate();

	///
	static void UnlockFramerate();


	static void Update();

private:
	static SystemEventHandler eventHandler;

};

#endif