#include "masterrenderer.h"

const char* MasterRenderer::shadowVertexPath = "shadow.vsh";
const char* MasterRenderer::shadowFragmentPath = "shadow.fsh";
const char* MasterRenderer::directionalLightVertexPath = "deferred/directional.vsh";
const char* MasterRenderer::directionalLightFragmentPath = "deferred/directional.fsh";
const char* MasterRenderer::postProcessVertexPath = "postprocessing.vsh";
const char* MasterRenderer::postProcessFragmentPath = "postprocessing.fsh";

MasterRenderer::MasterRenderer(const char* shaderDirectory) {

	string directory(shaderDirectory);

	directory += "/";

	rectangleVAO = GenerateRectangleVAO();

	geometryRenderer = new GeometryRenderer();

	shadowRenderer = new ShadowRenderer(
		(directory + string(shadowVertexPath)).c_str(),
		(directory + string(shadowFragmentPath)).c_str());

	directionalLightRenderer = new DirectionalLightRenderer(
		(directory + string(directionalLightVertexPath)).c_str(),
		(directory + string(directionalLightFragmentPath)).c_str());

	postProcessRenderer = new PostProcessRenderer(
		(directory + string(postProcessVertexPath)).c_str(),
		(directory + string(postProcessFragmentPath)).c_str());

}

void MasterRenderer::RenderScene(Window* window, RenderTarget* target, Camera* camera, Scene* scene) {

	geometryRenderer->Render(window, target, camera, scene, true);
	directionalLightRenderer->Render(window, target, camera, scene, true);
	postProcessRenderer->Render(window, target, camera, scene, true);

}

void MasterRenderer::RenderTexture(Texture* texture) {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void MasterRenderer::RenderRectangle(vec3 color) {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

uint32_t MasterRenderer::GenerateRectangleVAO() {

	uint32_t vao = 0;
	int8_t vertices[] = { -1, -1, 1, -1, -1, 1, 1, 1 };

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);

	uint32_t verticesBuffer;
	glGenBuffers(1, &verticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_BYTE, false, 0, NULL);

	return vao;

}