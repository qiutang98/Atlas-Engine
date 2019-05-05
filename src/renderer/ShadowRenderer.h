#ifndef AE_SHADOWRENDERER_H
#define AE_SHADOWRENDERER_H

#include "../System.h"
#include "../RenderList.h"
#include "Renderer.h"
#include "../shader/ShaderBatch.h"

#include <mutex>

namespace Atlas {

	namespace Renderer {

		class ShadowRenderer : public Renderer {

		public:
			ShadowRenderer();

			virtual void Render(Viewport* viewport, RenderTarget* target, Camera* camera, Scene::Scene* scene);

			static void InitShaderBatch();

			static void AddConfig(Shader::ShaderConfig* config);

			static void RemoveConfig(Shader::ShaderConfig* config);

			static std::string vertexPath;
			static std::string fragmentPath;

		private:
			RenderList renderList;

			Framebuffer framebuffer;

			Shader::Uniform* arrayMapUniform;
			Shader::Uniform* diffuseMapUniform;
			Shader::Uniform* lightSpaceMatrixUniform;
			Shader::Uniform* modelMatrixUniform;

			static Shader::ShaderBatch shaderBatch;
			static std::mutex shaderBatchMutex;

		};

	}

}

#endif