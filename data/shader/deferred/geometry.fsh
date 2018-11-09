layout (location = 0) out vec3 diffuse;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec2 additional;

#ifdef DIFFUSE_MAP
uniform sampler2D diffuseMap;
#endif
#ifdef NORMAL_MAP
uniform sampler2D normalMap;
#endif
#ifdef SPECULAR_MAP
uniform sampler2D specularMap;
#endif
#ifdef HEIGHT_MAP
uniform sampler2D heightMap;
#endif

#ifdef REFLECTION
uniform samplerCube environmentCube;
uniform mat4 ivMatrix;
#endif

uniform vec3 diffuseColor;
uniform float specularIntensity;
uniform float specularHardness;

in vec2 fTexCoord;
in vec3 fNormal;

#ifdef NORMAL_MAP
in mat3 toTangentSpace;
#endif


#ifdef REFLECTION
in vec3 fPosition;
#endif

void main() {
	
	vec4 textureColor = vec4(diffuseColor, 1.0f);
	
#ifdef DIFFUSE_MAP
	textureColor *= texture(diffuseMap, fTexCoord);
	
	if (textureColor.a < 0.2f)
		discard;
#endif

	normal = fNormal;

#ifdef NORMAL_MAP
	normal = normalize(toTangentSpace * (2.0f * texture(normalMap, fTexCoord).rgb - 1.0f));
#else
	normal = normalize(normal);
#endif

#ifdef REFLECTION
    vec3 R = mat3(ivMatrix) * reflect(normalize(fPosition), normal);
    diffuse = mix(textureColor.rgb, textureLod(environmentCube, R, specularHardness / 50.0f).rgb, reflectivity);
#else
	diffuse = textureColor.rgb;
#endif

	additional = vec2(specularIntensity, specularHardness);
	
	normal = 0.5f * normal + 0.5f;
	
}