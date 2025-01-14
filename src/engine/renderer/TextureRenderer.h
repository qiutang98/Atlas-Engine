#ifndef AE_TEXTURERENDERER_H
#define AE_TEXTURERENDERER_H

#include "Renderer.h"

#include "texture/Texture2D.h"
#include "texture/Texture2DArray.h"
#include "texture/Texture3D.h"

namespace Atlas {

	namespace Renderer {

		class TextureRenderer {

		public:
			TextureRenderer();

			void RenderTexture2D(Viewport* viewport, Texture::Texture2D* texture, float x, float y, float width, float height,
				bool alphaBlending = false, bool invert = false, Framebuffer* framebuffer = nullptr);

			void RenderTexture2D(Viewport* viewport, Texture::Texture2D* texture, float x, float y, float width, float height,
				vec4 clipArea, vec4 blendArea, bool alphaBlending = false, bool invert = false, Framebuffer* framebuffer = nullptr);

			void RenderTexture2DArray(Viewport* viewport, Texture::Texture2DArray* texture, int32_t depth, float x,
				float y, float width, float height, bool alphaBlending = false, bool invert = false, Framebuffer* framebuffer = nullptr);

			void RenderTexture2DArray(Viewport* viewport, Texture::Texture2DArray* texture, int32_t depth, float x, float y, float width, float height,
				vec4 clipArea, vec4 blendArea, bool alphaBlending = false, bool invert = false, Framebuffer* framebuffer = nullptr);

			void RenderTexture3D(Viewport* viewport, Texture::Texture3D* texture, float depth, float x,
				float y, float width, float height, bool alphaBlending = false, bool invert = false, Framebuffer* framebuffer = nullptr);

			void RenderTexture3D(Viewport* viewport, Texture::Texture3D* texture, float depth, float x, float y, float width, float height,
				vec4 clipArea, vec4 blendArea, bool alphaBlending = false, bool invert = false, Framebuffer* framebuffer = nullptr);

		private:
			Buffer::VertexArray vertexArray;

			Shader::Shader texture2DShader;
			Shader::Shader texture2DArrayShader;
			Shader::Shader texture3DShader;

		};

	}

}

#endif