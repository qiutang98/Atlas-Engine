#include "TerrainShadowRenderer.h"
#include "helper/GeometryHelper.h"

namespace Atlas {

	namespace Renderer {

		std::string TerrainShadowRenderer::vertexPath = "terrain/shadowmapping.vsh";
		std::string TerrainShadowRenderer::fragmentPath = "terrain/shadowmapping.fsh";

		TerrainShadowRenderer::TerrainShadowRenderer() {

			shader.AddStage(AE_VERTEX_STAGE, vertexPath);
			shader.AddStage(AE_FRAGMENT_STAGE, fragmentPath);

			shader.Compile();

			GetUniforms();

		}

		void TerrainShadowRenderer::Render(Viewport* viewport, RenderTarget* target, Camera* camera, Scene::Scene* scene) {

			if (!scene->terrain)
				return;

			auto terrain = scene->terrain;

			framebuffer.Bind();
			shader.Bind();
			terrain->distanceVertexArray.Bind();

			auto lights = scene->GetLights();

			for (auto light : lights) {

				if (!light->GetShadow()) {
					continue;
				}

				if (!light->GetShadow()->update ||
					!light->GetShadow()->allowTerrain) {
					continue;
				}

				glViewport(0, 0, light->GetShadow()->resolution, light->GetShadow()->resolution);

				for (int32_t i = 0; i < light->GetShadow()->componentCount; i++) {

					auto component = &light->GetShadow()->components[i];

					if (light->GetShadow()->useCubemap) {
						framebuffer.AddComponentCubemap(GL_DEPTH_ATTACHMENT, &light->GetShadow()->cubemap, i);
					}
					else {
						framebuffer.AddComponentTextureArray(GL_DEPTH_ATTACHMENT, &light->GetShadow()->maps, i);
					}

					auto inverseMatrix = glm::inverse(component->terrainFrustumMatrix);

					auto corners = GetFrustumCorners(inverseMatrix);
					auto frustum = Volume::Frustum(corners);

					mat4 lightSpace = component->projectionMatrix * component->viewMatrix;
					
					// Use middle of near plane as camera origin
					auto center = corners[4] + 0.5f * (corners[5] - corners[4])
						+ 0.5f * (corners[6] - corners[4]);

					terrain->UpdateRenderlist(&frustum, center);

					lightSpaceMatrix->SetValue(lightSpace);

					heightScale->SetValue(terrain->heightScale);

					for (auto node : terrain->renderList) {

						node->cell->heightField->Bind(GL_TEXTURE0);

						nodeLocation->SetValue(node->location);
						nodeSideLength->SetValue(node->sideLength);

						leftLoD->SetValue(node->leftLoDStitch);
						topLoD->SetValue(node->topLoDStitch);
						rightLoD->SetValue(node->rightLoDStitch);
						bottomLoD->SetValue(node->bottomLoDStitch);

						tileScale->SetValue(terrain->resolution * powf(2.0f, (float)(terrain->LoDCount - node->cell->LoD) - 1.0f));
						patchSize->SetValue((float)terrain->patchSizeFactor);

						glDrawElements(GL_TRIANGLE_STRIP, (int32_t)terrain->distanceVertexArray.GetIndexComponent()->GetElementCount(),
							terrain->distanceVertexArray.GetIndexComponent()->GetDataType(), nullptr);

					}

				}

			}

		}

		void TerrainShadowRenderer::GetUniforms() {

			heightScale = shader.GetUniform("heightScale");
			tileScale = shader.GetUniform("tileScale");
			patchSize = shader.GetUniform("patchSize");

			nodeLocation = shader.GetUniform("nodeLocation");
			nodeSideLength = shader.GetUniform("nodeSideLength");

			leftLoD = shader.GetUniform("leftLoD");
			topLoD = shader.GetUniform("topLoD");
			rightLoD = shader.GetUniform("rightLoD");
			bottomLoD = shader.GetUniform("bottomLoD");

			lightSpaceMatrix = shader.GetUniform("lightSpaceMatrix");

		}

		std::vector<vec3> TerrainShadowRenderer::GetFrustumCorners(mat4 inverseMatrix) {

			vec3 vectors[8] = {
				vec3(-1.0f, 1.0f, 1.0f),
				vec3(1.0f, 1.0f, 1.0f),
				vec3(-1.0f, -1.0f, 1.0f),
				vec3(1.0f, -1.0f, 1.0f),
				vec3(-1.0f, 1.0f, -1.0f),
				vec3(1.0f, 1.0f, -1.0f),
				vec3(-1.0f, -1.0f, -1.0f),
				vec3(1.0f, -1.0f, -1.0f)
			};

			std::vector<vec3> corners;

			for (uint8_t i = 0; i < 8; i++) {
				auto homogenous = inverseMatrix * vec4(vectors[i], 1.0f);
				corners.push_back(vec3(homogenous) / homogenous.w);
			}

			return corners;

		}

	}

}