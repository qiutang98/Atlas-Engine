#ifndef AE_MATERIAL_H
#define AE_MATERIAL_H

#include "System.h"
#include "texture/Texture2D.h"
#include "texture/Texture2DArray.h"
#include "loader/ImageLoader.h"
#include "shader/Shader.h"
#include "shader/ShaderConfig.h"

#include <string>

#define AE_MATERIAL_DIFFUSE_MAP 0
#define AE_MATERIAL_NORMAL_MAP 1
#define AE_MATERIAL_SPECULAR_MAP 2
#define AE_MATERIAL_DISPLACEMENT_MAP 3

namespace Atlas {

	class Material {

	public:
		Material();

		~Material();

		bool HasDiffuseMap();
		bool HasNormalMap();
		bool HasSpecularMap();
		bool HasDisplacementMap();

		std::string name;

		Texture::Texture2D* diffuseMap;
		Texture::Texture2D* normalMap;
		Texture::Texture2D* specularMap;
		Texture::Texture2D* displacementMap;

		vec3 diffuseColor;
		vec3 specularColor;
		vec3 ambientColor;

		float specularHardness;
		float specularIntensity;

		float displacementScale;

	};


}

#endif