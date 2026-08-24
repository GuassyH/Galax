#version 460 core

// TAKEN FROM SEBASTIAN LAGUE
// (While learning)

// Atmosphere
uniform float planetRadius;
uniform float atmosphereHeight;
uniform float intensity;
uniform float densityFalloff;

uniform vec3 centre;
uniform vec3 wavelengthScatter;

uniform sampler2D bakedOpticalTexture;

uniform mat4 invProjMat;

in vec2 texCoord;
out vec4 fragColor;

uniform vec3 camPos;
uniform vec3 sunPos;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

uniform vec2 screenResolution;

uniform vec3 camUp;
uniform vec3 camForward;
uniform vec3 camRight;
uniform float camNearPlane;

uniform float FOVdeg;


// RENDERING SETTINGS
uniform int numInScatteringPoints;
uniform int numOpticalDepthPoints;



/////////////////////////////////////
// Atmospheric Calculations
/////////////////////////////////////



// Check if a ray intersects with a sphere
vec2 raySphere (vec3 sphereCentre, float sphereRadius, vec3 rayOrigin, vec3 rayDir) {
    vec3 offset = rayOrigin - sphereCentre;
    const float a = 1; // set to dot(rayDir, rayDir) if rayDir might be unnormalized
    float b = 2 * dot (offset, rayDir);
    float c = dot (offset, offset) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4 * a * c;

    // No intersections: discriminant < 0	1 intersection: discriminant == 0	2 intersections: discriminant > 0
    if (discriminant > 0) {
        float s = sqrt(discriminant);
        float dstToSphereNear = max (0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

        if (dstToSphereFar >= 0) {
            return vec2 (dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }
    return vec2(0,0);
}


// Check what the density at a point would be
float densityAtPoint(float atmosphereRadius, vec3 samplePoint){
	float heightAbove = distance(samplePoint, centre) - planetRadius;
	float height01 = heightAbove / (atmosphereRadius - planetRadius);
	height01 = clamp(height01, 0.0, 1.0);

	if (height01 == 0)
		return 1.0;
	
	float localDensity = exp(-height01 * densityFalloff) * (1 - height01);

	return localDensity;
}

float opticalDepthBaked(vec3 rayOrigin, vec3 rayDir, float atmosphereRadius) {
	float height = length(rayOrigin - centre) - planetRadius;
	float height01 = clamp(height / (atmosphereRadius - planetRadius), 0, 1);

	// Theres a weird precision error in the creation of the opticaldepth tex. Therefore clamping is needed The border pixels arent rendered CHECK OPTICAL DEPTH BAKER
	float uvX = 1 - clamp(dot(normalize(rayOrigin - centre), rayDir) * 0.5 + 0.5, 0.001, 0.999);
	return textureLod(bakedOpticalTexture, vec2(uvX, clamp(height01, 0.001, 0.999)), 0).a;
}

float opticalDepthBaked2(vec3 rayOrigin, vec3 rayDir, float rayLength, float atmosphereRadius) {
	vec3 endPoint = rayOrigin + rayDir * rayLength;
	float d = dot(rayDir, normalize(rayOrigin-centre));
	float opticalDepth = 0;

	const float blendStrength = 1.5;
	float w = clamp(d * blendStrength + .5, 0, 1);
				
	float d1 = opticalDepthBaked(rayOrigin, rayDir, atmosphereRadius) - opticalDepthBaked(endPoint, rayDir, atmosphereRadius);
	float d2 = opticalDepthBaked(endPoint, -rayDir, atmosphereRadius) - opticalDepthBaked(rayOrigin, -rayDir, atmosphereRadius);

	opticalDepth = mix(d2, d1, w);
	return opticalDepth;
}
			

/////////////////////////////////////
// Calculation 
/////////////////////////////////////



// Light coming from the points along the ray
vec3 calculateLight(float atmosphereRadius, vec3 rayOrigin, vec3 rayDir, float dstThrough, vec3 originalCol){

	vec3 inScatterPoint = rayOrigin;
	vec3 inScatteredLight = vec3(0.0);
	
	float stepSize = dstThrough / (numInScatteringPoints + 1);
	float viewRayOpticalDepth = 0.0;

	vec3 dirToSun = normalize(sunPos - centre);
	
	for(int i = 0; i < numInScatteringPoints; i++){
		float sunRayLength = raySphere(centre, atmosphereRadius, inScatterPoint, dirToSun).y;
		float sunRayOpticalDepth = opticalDepthBaked2(inScatterPoint, dirToSun, sunRayLength, atmosphereRadius);

		float localDensity = densityAtPoint(atmosphereRadius, inScatterPoint);
		viewRayOpticalDepth = opticalDepthBaked2(rayOrigin, rayDir, stepSize * i, atmosphereRadius);
		vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * wavelengthScatter);
					
		inScatteredLight += localDensity * transmittance;
		inScatterPoint += rayDir * stepSize;
	}

	inScatteredLight *= wavelengthScatter * intensity * stepSize / planetRadius;
	
	const float brightnessAdaptionStrength = 0.15;
	const float reflectedLightOutScatterStrength = 3;
	float brightnessAdaption = dot (inScatteredLight, vec3(1)) * brightnessAdaptionStrength;
	float brightnessSum = viewRayOpticalDepth * intensity * reflectedLightOutScatterStrength + brightnessAdaption;
	float reflectedLightStrength = exp(-brightnessSum);
	float hdrStrength = clamp(((dot(originalCol, vec3(1)) / 3) -1), 0, 1);
	reflectedLightStrength = mix(reflectedLightStrength, 1, hdrStrength);
	vec3 reflectedLight = originalCol * reflectedLightStrength;

	vec3 finalCol = reflectedLight + inScatteredLight;

	return finalCol;
}



/////////////////////////////////////
// Depth
/////////////////////////////////////



// Convert from OpenGL depth to length zNear->zFar
float LinearizeDepth(float d, float nearPlane)
{
    return nearPlane / d;
}


vec3 ReconstructViewPos(vec2 uv, float depth)
{
    vec4 clip = vec4(
        uv * 2.0 - 1.0,
        depth,
        1.0
    );

    vec4 view = invProjMat * clip;
    view /= view.w;

    return view.xyz;
}

/////////////////////////////////////
// Main
/////////////////////////////////////



void main(){

	vec4 originalCol = texture(screenTexture, texCoord);

	// Get Raydir
	vec2 rayCoord = texCoord * 2.0 - 1.0;

	float fov = radians(FOVdeg); // adjust as needed
	float aspect = screenResolution.x / screenResolution.y;
	float scale = tan(fov * 0.5);

	vec3 rayDir = normalize(	camForward + rayCoord.x * aspect * scale * camRight + rayCoord.y * scale * camUp	);
	vec3 rayOrigin = vec3(0.0);
	vec3 localOrigin = centre - camPos;

	// Get Ray depths
	float depth = texture(depthTexture, texCoord).r;
	vec3 scenePos = ReconstructViewPos(texCoord, depth);
	float sceneDepthLinear = length(scenePos);

	float atmosphereRadius = planetRadius + atmosphereHeight;

	vec2 intersect = raySphere(localOrigin, atmosphereRadius, rayOrigin, rayDir); 
	float dstTo = intersect.x;
	float dstThrough = intersect.y;

	// If the ray intersects 
	if (dstThrough > 0.0) {
		// Clamp dstThrough to not exceed the scene
		dstThrough = min(dstThrough, sceneDepthLinear - dstTo);

		if(dstThrough > 0.0) {
			const float epsilon = 0.001;
			vec3 entryPoint = (rayDir * (dstTo + epsilon));
			vec3 light = calculateLight(atmosphereRadius, entryPoint + camPos, rayDir, dstThrough - (epsilon * 2), originalCol.rgb);

			fragColor = vec4(light, 1.0);
			return;
		}
	}

	fragColor = originalCol;
} 