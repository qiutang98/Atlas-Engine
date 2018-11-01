#ifndef POSTPROCESSRENDERER_H
#define POSTPROCESSRENDERER_H

#include "../system.h"
#include "IRenderer.h"
#include "../shader/shader.h"

class PostProcessRenderer : public IRenderer {

public:
	PostProcessRenderer(const char* vertexSource, const char* fragmentSource);

	virtual void Render(Window* window, RenderTarget* target, Camera* camera, Scene* scene);

private:
	uint32_t rectangleVAO;

	Shader* shader;

	Uniform* exposure;
	Uniform* saturation;

};

#endif