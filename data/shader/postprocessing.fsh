in vec2 fPosition;

layout(binding = 0) uniform sampler2D hdrTexture;
layout(binding = 1) uniform sampler2D bloomFirstTexture;
layout(binding = 2) uniform sampler2D bloomSecondTexture;
layout(binding = 3) uniform sampler2D bloomThirdTexture;

uniform vec2 hdrTextureResolution;
uniform float exposure;
uniform float saturation;
uniform float timeInMilliseconds;

#ifdef BLOOM
uniform int bloomPasses;
#endif
#ifdef CHROMATIC_ABERRATION
uniform float aberrationStrength;
uniform float aberrationReversed;
#endif
#ifdef VIGNETTE
uniform float vignetteOffset;
uniform float vignettePower;
uniform float vignetteStrength;
uniform vec3 vignetteColor;
#endif

out vec3 color;

const float gamma = 1.0 / 2.2;

vec3 ACESToneMap(vec3 hdrColor) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((hdrColor*(a*hdrColor+b))/
		(hdrColor*(c*hdrColor+d)+e), 0.0, 1.0);
}

vec3 ToneMap(vec3 hdrColor) {
	
	return vec3(1.0) - exp(-hdrColor);
	
}

vec3 saturate(vec3 color, float factor) {
	const vec3 luma = vec3(0.299, 0.587, 0.114);
    vec3 pixelLuminance = vec3(dot(color, luma));
	return mix(pixelLuminance, color, factor);
}


//note: uniformly distributed, normalized rand, [0;1[
float nrand( vec2 n )
{
	return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}
//note: remaps v to [0;1] in interval [a;b]
float remap( float a, float b, float v )
{
	return clamp( (v-a) / (b-a), 0.0, 1.0 );
}
//note: quantizes in l levels
float truncf( float a, float l )
{
	return floor(a*l)/l;
}

float n2rand( vec2 n )
{
	float t = fract(timeInMilliseconds/1000000.0f);
	float nrnd0 = nrand( n + 0.07*t );
	float nrnd1 = nrand( n + 0.11*t );
	return (nrnd0+nrnd1) / 2.0;
}

void main() {
	
	vec2 fTexCoord = 0.5 * fPosition + 0.5;
	fTexCoord.y = 1.0 - fTexCoord.y;
	
#ifdef CHROMATIC_ABERRATION
	vec2 uvRedChannel = (fPosition - fPosition * 0.005f * aberrationStrength 
		* aberrationReversed) * 0.5f + 0.5f;
	vec2 uvGreenChannel = (fPosition - fPosition * 0.0025f * aberrationStrength) * 0.5f + 0.5f;
	vec2 uvBlueChannel =  (fPosition - fPosition * 0.005f * aberrationStrength
		* (1.0f - aberrationReversed)) * 0.5f + 0.5f;
		
	uvRedChannel.y = 1.0 - uvRedChannel.y;
	uvGreenChannel.y = 1.0 - uvGreenChannel.y;
	uvBlueChannel.y = 1.0 - uvBlueChannel.y;	
	
	color.r = texture(hdrTexture, uvRedChannel).r;
	color.g = texture(hdrTexture, uvGreenChannel).g;
	color.b = texture(hdrTexture, uvBlueChannel).b;
	
#ifdef BLOOM
    // We want to keep a constant expression in texture[const]
	// because OpenGL ES doesn't support dynamic texture fetches
	// inside a loop
	if (bloomPasses > 0) {
		color.r += texture(bloomFirstTexture, uvRedChannel).r;
		color.g += texture(bloomFirstTexture, uvGreenChannel).g;
		color.b += texture(bloomFirstTexture, uvBlueChannel).b;
	}
	if (bloomPasses > 1) {
		color.r += texture(bloomSecondTexture, uvRedChannel).r;
		color.g += texture(bloomSecondTexture, uvGreenChannel).g;
		color.b += texture(bloomSecondTexture, uvBlueChannel).b;
	}
	if (bloomPasses > 2) {
		color.r += texture(bloomThirdTexture, uvRedChannel).r;
		color.g += texture(bloomThirdTexture, uvGreenChannel).g;
		color.b += texture(bloomThirdTexture, uvBlueChannel).b;
	}
#endif
#else
	color = texture(hdrTexture, fTexCoord).rgb;
#ifdef BLOOM
	if (bloomPasses > 0) {
		color += texture(bloomFirstTexture, fTexCoord).rgb;
	}
	if (bloomPasses > 1) {
		color += texture(bloomSecondTexture, fTexCoord).rgb;
	}
	if (bloomPasses > 2) {
		color += texture(bloomThirdTexture, fTexCoord).rgb;
	}
#endif
#endif

	color *= exposure;
	
	//color = color + n2rand(2.0 * fTexCoord - 1.0) / 256.0;
	
	// Apply the tone mapping because we want the colors to be back in
	// normal range
#ifdef FILMIC_TONEMAPPING
	color = ACESToneMap(color);
#else
	color = ToneMap(color);
#endif
	color = pow(color, vec3(gamma));
	
	color = clamp(saturate(color, saturation), vec3(0.0), vec3(1.0));	

#ifdef VIGNETTE	
	float vignetteFactor = max(1.0 - max(pow(length(fPosition) - vignetteOffset, vignettePower), 0.0)
		* vignetteStrength, 0.0);
	
	color = mix(vignetteColor, color, vignetteFactor);
#endif
	//color = vec3(texture(hdrTexture, fTexCoord).a);
	
}