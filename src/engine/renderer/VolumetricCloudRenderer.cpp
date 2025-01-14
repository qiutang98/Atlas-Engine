#include "VolumetricCloudRenderer.h"

#include "common/RandomHelper.h"
#include "../Clock.h"

namespace Atlas {

	namespace Renderer {

		VolumetricCloudRenderer::VolumetricCloudRenderer() {

			auto noiseImage = Loader::ImageLoader::LoadImage<uint8_t>("noise.png");
			blueNoiseTexture = Texture::Texture2D(noiseImage.width, noiseImage.height, GL_RGBA8, GL_REPEAT, GL_NEAREST);
			blueNoiseTexture.SetData(noiseImage.GetData());

			shapeNoiseShader.AddStage(AE_COMPUTE_STAGE, "clouds/shapeNoise.csh");
			shapeNoiseShader.Compile();

			detailNoiseShader.AddStage(AE_COMPUTE_STAGE, "clouds/detailNoise.csh");
			detailNoiseShader.Compile();

			integrateShader.AddStage(AE_COMPUTE_STAGE, "clouds/integrate.csh");
			integrateShader.Compile();

			temporalShader.AddStage(AE_COMPUTE_STAGE, "clouds/temporal.csh");
			temporalShader.Compile();

		}

		void VolumetricCloudRenderer::Render(Viewport* viewport, RenderTarget* target,
			Camera* camera, Scene::Scene* scene) {

			auto clouds = scene->sky.clouds;
			auto sun = scene->sky.sun;
			if (!clouds) return;

			Profiler::BeginQuery("Volumetric clouds");

			if (clouds->needsNoiseUpdate) {
				GenerateTextures(scene);
				clouds->needsNoiseUpdate = false;
			}

			auto downsampledRT = target->GetDownsampledTextures(target->GetVolumetricResolution());

			ivec2 res = ivec2(target->volumetricCloudsTexture.width, target->volumetricCloudsTexture.height);

			auto depthTexture = downsampledRT->depthTexture;
			auto velocityTexture = downsampledRT->velocityTexture;

			{
				Profiler::BeginQuery("Integrate");

				ivec2 groupCount = ivec2(res.x / 8, res.y / 4);
				groupCount.x += ((groupCount.x * 8 == res.x) ? 0 : 1);
				groupCount.y += ((groupCount.y * 4 == res.y) ? 0 : 1);

				integrateShader.Bind();

				target->swapVolumetricCloudsTexture.Bind(GL_WRITE_ONLY, 0);
				depthTexture->Bind(0);
				clouds->shapeTexture.Bind(1);
				clouds->detailTexture.Bind(2);
				blueNoiseTexture.Bind(3);

				integrateShader.GetUniform("vMatrix")->SetValue(camera->viewMatrix);
				integrateShader.GetUniform("pMatrix")->SetValue(camera->projectionMatrix);
				integrateShader.GetUniform("ipMatrix")->SetValue(camera->invProjectionMatrix);
				integrateShader.GetUniform("ivMatrix")->SetValue(camera->invViewMatrix);
				integrateShader.GetUniform("cameraLocation")->SetValue(camera->GetLocation());
				
				integrateShader.GetUniform("densityMultiplier")->SetValue(clouds->densityMultiplier);

				integrateShader.GetUniform("planetRadius")->SetValue(scene->sky.planetRadius);
				integrateShader.GetUniform("innerRadius")->SetValue(scene->sky.planetRadius + clouds->minHeight);
				integrateShader.GetUniform("outerRadius")->SetValue(scene->sky.planetRadius + clouds->maxHeight);
				integrateShader.GetUniform("distanceLimit")->SetValue(clouds->distanceLimit);

				integrateShader.GetUniform("lowerHeightFalloff")->SetValue(clouds->lowerHeightFalloff);
				integrateShader.GetUniform("upperHeightFalloff")->SetValue(clouds->upperHeightFalloff);

				integrateShader.GetUniform("shapeScale")->SetValue(clouds->shapeScale);
				integrateShader.GetUniform("detailScale")->SetValue(clouds->detailScale);
				integrateShader.GetUniform("shapeSpeed")->SetValue(clouds->shapeSpeed);
				integrateShader.GetUniform("detailSpeed")->SetValue(clouds->detailSpeed);
				integrateShader.GetUniform("detailStrength")->SetValue(clouds->detailStrength);

				integrateShader.GetUniform("eccentricity")->SetValue(clouds->scattering.eccentricity);
				integrateShader.GetUniform("extinctionFactor")->SetValue(clouds->scattering.extinctionFactor);
				integrateShader.GetUniform("scatteringFactor")->SetValue(clouds->scattering.scatteringFactor);

				integrateShader.GetUniform("silverLiningSpread")->SetValue(clouds->silverLiningSpread);
				integrateShader.GetUniform("silverLiningIntensity")->SetValue(clouds->silverLiningIntensity);

				if (sun) {
					integrateShader.GetUniform("light.direction")->SetValue(sun->direction);
					integrateShader.GetUniform("light.color")->SetValue(sun->color);
					integrateShader.GetUniform("light.intensity")->SetValue(sun->intensity);
				}
				else {
					integrateShader.GetUniform("light.intensity")->SetValue(0.0f);
				}

				integrateShader.GetUniform("time")->SetValue(Clock::Get());
				integrateShader.GetUniform("frameSeed")->SetValue(Common::Random::SampleUniformInt(0, 255));

				glDispatchCompute(groupCount.x, groupCount.y, 1);

				Profiler::EndQuery();
			}

			{
				Profiler::BeginQuery("Temporal accumulation");

				ivec2 groupCount = ivec2(res.x / 8, res.y / 8);
				groupCount.x += ((groupCount.x * 8 == res.x) ? 0 : 1);
				groupCount.y += ((groupCount.y * 8 == res.y) ? 0 : 1);

				temporalShader.Bind();

				target->swapVolumetricCloudsTexture.Bind(0);
				velocityTexture->Bind(1);
				depthTexture->Bind(2);
				target->historyVolumetricCloudsTexture.Bind(3);
				target->volumetricCloudsTexture.Bind(GL_WRITE_ONLY, 0);

				temporalShader.GetUniform("invResolution")->SetValue(1.0f / vec2((float)res.x, (float)res.y));
				temporalShader.GetUniform("resolution")->SetValue(vec2((float)res.x, (float)res.y));

				glMemoryBarrier(GL_ALL_BARRIER_BITS);
				glDispatchCompute(groupCount.x, groupCount.y, 1);

				Profiler::EndQuery();
			}

			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			target->historyVolumetricCloudsTexture = target->volumetricCloudsTexture;
			
			Profiler::EndQuery();

		}

		void VolumetricCloudRenderer::GenerateTextures(Scene::Scene* scene) {

			auto clouds = scene->sky.clouds;
			if (!clouds) return;

			GenerateShapeTexture(&clouds->shapeTexture, clouds->shapeScale);
			GenerateDetailTexture(&clouds->detailTexture, clouds->detailScale);

			clouds->shapeTexture.GenerateMipmap();
			clouds->detailTexture.GenerateMipmap();

		}

		void VolumetricCloudRenderer::GenerateShapeTexture(Texture::Texture3D* texture, float baseScale) {

			Profiler::BeginQuery("Generate shape cloud texture");

			// Expect the resolution to be a power of 2 and larger equal 4
			ivec3 groupCount = ivec3(texture->width, texture->height, texture->depth) / 4;

			shapeNoiseShader.Bind();

			texture->Bind(GL_WRITE_ONLY, 0);

			shapeNoiseShader.GetUniform("seed")->SetValue(Common::Random::SampleUniformFloat() * 10.0f);
			
			glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);

			Profiler::EndQuery();

		}

		void VolumetricCloudRenderer::GenerateDetailTexture(Texture::Texture3D* texture, float baseScale) {

			Profiler::BeginQuery("Generate detail cloud texture");

			// Expect the resolution to be a power of 2 and larger equal 4
			ivec3 groupCount = ivec3(texture->width, texture->height, texture->depth) / 4;

			detailNoiseShader.Bind();

			texture->Bind(GL_WRITE_ONLY, 0);

			detailNoiseShader.GetUniform("seed")->SetValue(Common::Random::SampleUniformFloat() * 10.0f);

			glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);

			Profiler::EndQuery();

		}

	}

}