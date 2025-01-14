layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 velocity;

#define PI 3.141592
#define iSteps 20
#define jSteps 10

in vec3 fPosition;
in vec3 ndcCurrent;
in vec3 ndcLast;

uniform vec3 cameraLocation;
uniform vec3 sunDirection;
uniform float sunIntensity;
uniform float atmosphereRadius;
uniform float planetRadius;
uniform vec2 jitterLast;
uniform vec2 jitterCurrent;

const float rayScaleHeight = 8.0e3;
const float mieScaleHeight = 1.2e3; 

vec3 planetCenter = -vec3(0.0, planetRadius, 0.0);

void atmosphere(vec3 r, vec3 r0, vec3 pSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, out vec3 totalRlh, out vec3 totalMie);

void main() {
	
	const float g = 0.76;
	vec3 r = normalize(fPosition);
	vec3 pSun = normalize(-sunDirection);
	
	vec3 totalRlh;
	vec3 totalMie;
	
	atmosphere(
        normalize(fPosition),           // normalized ray direction
        cameraLocation,               // ray origin
        -sunDirection,                        // position of the sun
        planetRadius,                         // radius of the planet in meters
        atmosphereRadius,                         // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6,                          // Mie scattering coefficient
		totalRlh,
		totalMie
    );	
		
	// Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));
	
	fragColor = max(pRlh * totalRlh + pMie * totalMie, vec3(0.0));

	float LdotV = max(0.0, dot(normalize(pSun), normalize(r)));
	float sunAngle = cos(2.0 * 0.5 * 3.14 / 180.0);

	if (LdotV > sunAngle) {
		float sunDisk = clamp(0.5 * (LdotV - sunAngle) / (1.0 - sunAngle), 0.0, 1.0);
		fragColor += sunIntensity * sunDisk;
	}	
	
	// Calculate velocity
	// Calculate velocity
	vec2 ndcL = ndcLast.xy / ndcLast.z;
	vec2 ndcC = ndcCurrent.xy / ndcCurrent.z;

	ndcL -= jitterLast;
	ndcC -= jitterCurrent;

	velocity = (ndcL - ndcC) * 0.5;
	
}

vec2 IntersectSphere(vec3 origin, vec3 direction, vec3 pos, float radius) {

	vec3 L = pos - origin;
	float DT = dot(L, direction);
	float r2 = radius * radius;
	
	float ct2 = dot(L, L) - DT * DT;
	
	if (ct2 > r2)
		return vec2(-1.0);
	
	float AT = sqrt(r2 - ct2);
	float BT = AT;
	
	float AO = DT - AT;
	float BO = DT + BT;

    float minDist = min(AO, BO);
    float maxDist = max(AO, BO);

    return vec2(minDist, maxDist);
}

void CalculateRayLength(vec3 rayOrigin, vec3 rayDirection, out float minDist, out float maxDist) {

    vec2 planetDist = IntersectSphere(rayOrigin, rayDirection, planetCenter, planetRadius);
    vec2 atmosDist = IntersectSphere(rayOrigin, rayDirection, planetCenter, atmosphereRadius);

    // We're in the in the planet
    if (planetDist.x < 0.0 && planetDist.y >= 0.0) {
        // When the planet is in front of the inner layer set dist to zero
        minDist = 0.0;
        maxDist = 0.0;
    }
    else {
		// We're in the atmosphere layer
		if (atmosDist.x < 0.0 && atmosDist.y >= 0.0) {
			// When the planet is in front of the inner layer set dist to zero
			minDist = 0.0;
			maxDist = planetDist.x >= 0.0 ? min(planetDist.x, atmosDist.y) : atmosDist.y;
    	}
		else {
			// Out of the atmosphere
			minDist = max(0.0, atmosDist.x);
			maxDist = planetDist.x >= 0.0 ? max(0.0, planetDist.x) : max(0.0, atmosDist.y);
		}
    }

}

bool LightSampling(vec3 origin, 
	vec3 sunDirection,
	float planetRadius, 
	float atmosRadius, 
	out float opticalDepthMie,
	out float opticalDepthRay) {
	
	float inDist, outDist;
	CalculateRayLength(origin, sunDirection, inDist, outDist);
	
	float time = 0.0;
	
	opticalDepthMie = 0.0;
	opticalDepthRay = 0.0;
	
	float stepSize = (outDist - inDist) / float(jSteps);
	
	for (int i = 0; i < jSteps; i++) {
		
		vec3 pos = origin + sunDirection * (time + 0.5 * stepSize);
		
		float height = distance(planetCenter, pos) - planetRadius;
		
		if (height < 0.0)
			return false;
		
		opticalDepthMie += exp(-height / mieScaleHeight) * stepSize;
		opticalDepthRay += exp(-height / rayScaleHeight) * stepSize;
		
		time += stepSize;
		
	}	
	
	return true;
	
}

void atmosphere(vec3 r, vec3 r0, vec3 pSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, out vec3 totalRlh, out vec3 totalMie) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);
	
	totalRlh = vec3(0.0);
    totalMie = vec3(0.0);

	float inDist, outDist;
	CalculateRayLength(r0, r, inDist, outDist);
	if (inDist <= 0.0 && outDist <= 0.0)
        return;
	
    float iStepSize = (outDist - inDist) / float(iSteps);

    // Initialize the primary ray time.
    float iTime = inDist;

    // Initialize accumulators for Rayleigh and Mie scattering.

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;
	
    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {

        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = distance(iPos, planetCenter) - rPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / rayScaleHeight) * iStepSize;
        float odStepMie = exp(-iHeight / mieScaleHeight) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        bool overground = LightSampling(iPos, pSun, rPlanet, rAtmos, jOdMie, jOdRlh);
		
		if (overground) {
		    // Calculate attenuation.
			vec3 transmittance = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

			// Accumulate scattering.
			totalRlh += odStepRlh * transmittance;
			totalMie += odStepMie * transmittance;		
		}

        // Increment the primary ray time.
        iTime += iStepSize;

    }
	
	totalMie = totalMie * sunIntensity * kMie;
	totalRlh = totalRlh * sunIntensity * kRlh;
	
}