#version 460 core


uniform sampler2D bakedOpticalTexture;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

uniform float cloudMin;
uniform float cloudMax;

uniform vec2 screenResolution;
uniform float FOVdeg;

uniform vec3 camForward;
uniform vec3 camUp;
uniform vec3 camRight;
uniform vec3 camPos;
uniform mat4 invProjMat;

uniform vec3 centre;
uniform vec3 sunPos;

in vec2 texCoord;

/// NOISE

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
vec3 hash(vec3 p) {   return fract(sin(vec3(dot(p, vec3(1.0, 57.0, 113.0)), dot(p, vec3(57.0, 113.0, 1.0)), dot(p, vec3(113.0, 1.0, 57.0)))) * 43758.5453); }

float noise(float x) {
	float i = floor(x);
	float f = fract(x);
	float u = f * f * (3.0 - 2.0 * f); // good way to do seed??
	return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}



float PerlinNoise(vec3 x) {
	const vec3 step = vec3(110, 241, 171);

	vec3 i = floor(x);
	vec3 f = fract(x);
 
	
    float n = dot(i, step);

	vec3 u = f * f * (3.0 - 2.0 * f);
	float result = mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);

	return result;
}



/// MAIN

float densityAtPoint(vec3 point){
    float val = (PerlinNoise(point / 200) * 0.04) + (PerlinNoise(point / 100) * 0.02);


    return val;
}

/// KEEP IN MIND, THERE CAN BE A SPACE IN THE MIDDLE OF THE RAY THAT IS EMPTY
vec3 calculateLight(float dstThrough, float dstTo, int numIntersections, vec3 rayOrigin, vec3 rayDir, vec3 originalCol){
    int numSteps = 10 * 2; // needs to be multple of two
    vec3 entryPoint = rayOrigin + (rayDir * dstTo) + 0.0002; // epsilon
    vec3 stepSize = rayDir * (dstThrough / numSteps);

    vec3 inScatteredLight;

    if (numIntersections == 1){
        for (int i = 0; i < numSteps; i++) {
            vec3 point = entryPoint + (stepSize * i);

            inScatteredLight += densityAtPoint(point);
        }
    }

    else if (numIntersections == 2){
        for (int i = 0; i < numSteps / 2; i++) {
            vec3 point = entryPoint + (stepSize * i);

            inScatteredLight += densityAtPoint(point);
        }

        for (int i = 0; i < numSteps / 2; i++) {
            vec3 point = entryPoint + (dstThrough / 2) + (stepSize * i);

            inScatteredLight += densityAtPoint(point);
        }
    }
    

    vec3 finalCol = mix(originalCol, vec3(0.8), inScatteredLight);

    return finalCol;
}

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


vec3 ReconstructViewPos(vec2 uv, float depth)
{
    vec4 clip = vec4(uv * 2.0 - 1.0, depth, 1.0);

    vec4 view = invProjMat * clip;
    view /= view.w;

    return view.xyz;
}


out vec4 fragCol;

void main(){
	vec4 originalCol = texture(screenTexture, texCoord);
	fragCol = originalCol;

	// Get Raydir
	vec2 rayCoord = texCoord * 2.0 - 1.0;

	float fov = radians(FOVdeg); // adjust as needed
	float aspect = screenResolution.x / screenResolution.y;
	float scale = tan(fov * 0.5);

	vec3 rayDir = normalize(	camForward + rayCoord.x * aspect * scale * camRight + rayCoord.y * scale * camUp	);
	vec3 rayOrigin = vec3(0.0);
	vec3 localOrigin = centre - camPos;

    vec2 maxIntersect = raySphere(localOrigin, cloudMax, rayOrigin, rayDir); 
    
    if (maxIntersect.y <= 0)
        return;
    
    // Get depth
    float depth = texture(depthTexture, texCoord).r;
	vec3 scenePos = ReconstructViewPos(texCoord, depth);
	float sceneDepthLinear = length(scenePos);

    vec2 minIntersect = raySphere(localOrigin, cloudMin, rayOrigin, rayDir); 

    float dstTo_Max = maxIntersect.x;
    float dstTo_Min = minIntersect.x;

    // Get distance through the cloud "shell" 
    float dstThrough_Max = min(maxIntersect.y, sceneDepthLinear - dstTo_Max);
    float dstThrough_Min = min(minIntersect.y, sceneDepthLinear - dstTo_Min);

    float dstThrough = dstThrough_Max - max(dstThrough_Min, 0); 

    if (dstThrough <= 0)
        return;


    int numIntersections = 0;
    if (minIntersect.y > sceneDepthLinear - minIntersect.x)
        numIntersections = 1;
    else
        numIntersections = 2;

    float closestDstTo = min(dstTo_Max, dstTo_Min);
    fragCol.rgb = calculateLight(dstThrough, closestDstTo, numIntersections, -localOrigin, rayDir, originalCol.rgb);

}