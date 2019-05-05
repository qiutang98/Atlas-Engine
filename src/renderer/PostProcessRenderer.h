#ifndef AE_POSTPROCESSRENDERER_H
#define AE_POSTPROCESSRENDERER_H

#include "../System.h"
#include "Renderer.h"
#include "../shader/Shader.h"

namespace Atlas {

	namespace Renderer {

		class PostProcessRenderer : public Renderer {

		public:
			PostProcessRenderer();

			virtual void Render(Viewport* viewport, RenderTarget* target, Camera* camera, Scene::Scene* scene);

			static std::string vertexPath;
			static std::string fragmentPath;

		private:
			void GetUniforms();

			Shader::Shader shader;

			Shader::Uniform* hdrTextureResolution;
			Shader::Uniform* exposure;
			Shader::Uniform* saturation;
			Shader::Uniform* bloomPassses;
			Shader::Uniform* aberrationStrength;
			Shader::Uniform* aberrationReversed;
			Shader::Uniform* vignetteOffset;
			Shader::Uniform* vignettePower;
			Shader::Uniform* vignetteStrength;
			Shader::Uniform* vignetteColor;
			Shader::Uniform* timeInMilliseconds;

		};

	}

}

#endif