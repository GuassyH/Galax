#version 460 core


// Atmosphere
uniform float planetRadius;
uniform float atmosphereHeight;
uniform float intensity;
uniform float densityFalloff;

uniform vec3 centre;
uniform vec3 wavelengths;
uniform vec3 scatteringCoefficients;

uniform float scatteringStrength;



in vec2 fragCoord;
out vec4 fragColor;


uniform vec3 camPos;
uniform vec3 sunPos;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform sampler2D starTexture;

uniform vec2 screenResolution;

uniform vec3 camUp;
uniform vec3 camForward;
uniform vec3 camRight;
uniform float camFarPlane;
uniform float camNearPlane;

uniform float FOVdeg;


// RENDERING SETTINGS
uniform int numInScatteringPoints;
uniform int numOpticalDepthPoints;


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




float densityAtPoint(float atmosphereRadius, vec3 samplePoint){
	float heightAbove = distance(samplePoint, centre) - planetRadius;
	float height01 = heightAbove / (atmosphereRadius - planetRadius);
	height01 = clamp(height01, 0.0, 1.0);

	float localDensity = exp(-height01 * densityFalloff) * (1 - height01);

	return localDensity;
}




// Light coming from the sun to the point
float opticalDepth(float atmosphereRadius, vec3 rayOrigin, vec3 rayDir, float sunRayLength){
	vec3 densitySamplePoint = rayOrigin;
	float stepSize = sunRayLength / (numOpticalDepthPoints - 1);
	float opticalDepth = 0;

	for(int i = 0; i < numOpticalDepthPoints; i++){
		float localDensity = densityAtPoint(atmosphereRadius, densitySamplePoint);
		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDir * stepSize;
	}

	return opticalDepth;
}


// Light coming from the points along the ray
vec3 calculateLight(float atmosphereRadius, vec3 rayOrigin, vec3 rayDir, float dstThrough, vec3 originalCol){

	vec3 inScatterPoint = rayOrigin;
	vec3 inScatteredLight = vec3(0.0);
	
	float stepSize = dstThrough / (numInScatteringPoints - 1);
	float viewRayOpticalDepth = 0.0;

	vec3 dirToSun = normalize(sunPos - centre);
	
	for(int i = 0; i < numInScatteringPoints; i++){
		
		float sunRayLength = raySphere(centre, atmosphereRadius, inScatterPoint, dirToSun).y;
		float sunRayOpticalDepth = opticalDepth(atmosphereRadius, inScatterPoint, dirToSun, sunRayLength);
		viewRayOpticalDepth = opticalDepth(atmosphereRadius, inScatterPoint, -rayDir, stepSize * i);
		vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatteringCoefficients);
		float localDensity = densityAtPoint(atmosphereRadius, inScatterPoint);

		inScatteredLight += localDensity * transmittance * scatteringCoefficients * stepSize;
		inScatterPoint += rayDir * stepSize;
	}

	inScatteredLight *= intensity;
	float originalColTransmittance = exp(-viewRayOpticalDepth);

	// return originalCol * originalColTransmittance + inScatteredLight;
	return inScatteredLight;
}



float LinearizeDepth(float d,float zNear,float zFar)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}


vec4 GetStarBrightness(float brightness){
	vec4 result = vec4(0.0);
	float weighted_brightness = clamp((1 - (brightness * 4)), 0, 1);

	result.rgb = vec3(texture(starTexture, fragCoord).r * weighted_brightness);

	return result;
}


void main(){
	fragColor = texture(screenTexture, fragCoord);

	vec2 rayCoord = fragCoord * 2.0 - 1.0;

	float fov = radians(FOVdeg); // adjust as needed
	float aspect = screenResolution.x / screenResolution.y;
	float scale = tan(fov * 0.5);

	vec3 rayDir = normalize(	camForward + rayCoord.x * aspect * scale * camRight + rayCoord.y * scale * camUp	);
	vec3 rayOrigin = camPos;

	float sceneDepthLinear = LinearizeDepth(texture(depthTexture, fragCoord).r, camNearPlane, camFarPlane);

		
	float atmosphereRadius = planetRadius + atmosphereHeight;

	vec2 intersect = raySphere(centre, atmosphereRadius, rayOrigin, rayDir); 
	float dstTo = intersect.x;
	float dstThrough = intersect.y;

	if (dstThrough > 0.0) {

		// Distance from camera along the current ray
		float distanceAlongRayToScene = sceneDepthLinear / dot(rayDir, camForward);

		// Clamp dstThrough to not exceed the scene
		dstThrough = min(dstThrough, distanceAlongRayToScene - dstTo);

		if(dstThrough > 0.0) {
			const float epsilon = 0.001;
			vec3 entryPoint = rayOrigin + (rayDir * (dstTo + epsilon));
			vec3 light = calculateLight(atmosphereRadius, entryPoint, rayDir, dstThrough - (epsilon * 2), vec3(fragColor));

			fragColor += vec4(light, 0.0);
		}
	}

	if(texture(depthTexture, fragCoord).r == 1){
		// float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
		// fragColor += GetStarBrightness(brightness);
		fragColor += GetStarBrightness(length(fragColor.rgb));
	}
} 