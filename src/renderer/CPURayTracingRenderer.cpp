#include "CPURayTracingRenderer.h"

#include "../common/Ray.h"

#include <vector>

namespace Atlas {

	namespace Renderer {

		std::string CPURayTracingRenderer::unprojectionComputePath = "raytracing/unprojection.csh";

		CPURayTracingRenderer::CPURayTracingRenderer() {



		}

		void CPURayTracingRenderer::Render(Viewport* window, RenderTarget* target, Camera* camera, Scene::Scene* scene) {

			

		}

		void CPURayTracingRenderer::Render(Viewport* viewport, Texture::Texture2D* texture, Camera* camera, Scene::Scene* scene) {

			auto data = texture->GetData();

			auto offset = ivec2(viewport->x, viewport->y);
			auto size = ivec2(viewport->width, viewport->height);

			std::vector<Actor::MeshActor*> actors;

			Common::AABB base(vec3(-1.0f), vec3(1.0f));
			auto aabb = base.Transform(glm::inverse(camera->projectionMatrix * camera->viewMatrix));

			auto movableActors = scene->GetMovableMeshActors(aabb);
			auto staticActors = scene->GetStaticMeshActors(aabb);

			for (auto& actor : movableActors)
				actors.push_back(actor);
			for (auto& actor : staticActors)
				actors.push_back(actor);

			for (int32_t x = 0; x < size.x; x++) {
				for (int32_t y = 0; y < size.y; y++) {

					auto pixel = offset + ivec2(x, y);
					auto nearPoint = viewport->Unproject(vec3((float)pixel.x, (float)pixel.y, 0.0f), camera);
					auto farPoint = viewport->Unproject(vec3((float)pixel.x, (float)pixel.y, 1.0f), camera);

					Common::Ray(nearPoint, glm::normalize(farPoint - nearPoint));

					data[(pixel.y * texture->width + pixel.x) * 3] = 0;
					data[(pixel.y * texture->width + pixel.x) * 3 + 1] = 0;
					data[(pixel.y * texture->width + pixel.x) * 3 + 2] = 0;

				}
			}

			texture->SetData(data);

		}

	}

}