#ifndef AE_RENDERTARGET_H
#define AE_RENDERTARGET_H

#include "System.h"
#include "Framebuffer.h"

namespace Atlas {

	class RenderTarget {

	public:
		/**
		 * Constructs a RenderTarget object.
		 */
		RenderTarget() {}

		/**
		 * Constructs a RenderTarget object.
		 * @param width The width of the render target
		 * @param height The height of the render target
		 */
		RenderTarget(int32_t width, int32_t height);

		/**
		 * Resizes the render target.
		 * @param width The width of the render target
		 * @param height The height of the render target
		 */
		void Resize(int32_t width, int32_t height);

		/**
		 * Returns the size of the render target.
		 * @return An 2-component integer vector where x is the width and y is the height.
		 */
		ivec2 GetSize();

		Framebuffer geometryFramebuffer;
		Framebuffer lightingFramebuffer;

	private:
		Texture::Texture2D depthTexture;

		int32_t width = 0;
		int32_t height = 0;

	};

}


#endif