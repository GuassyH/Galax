#version 460 core


uniform sampler2D depthTexture;
uniform sampler2D screenTexture;

uniform float oceanRadius;
uniform float densityFalloff;
uniform float camFarPlane;
uniform float camNearPlane;
uniform float FOVdeg;

uniform vec3 centre;
uniform vec3 camPos;
uniform vec3 sunPos;
uniform vec3 camUp;
uniform vec3 camForward;
uniform vec3 camRight;

uniform mat4 viewMat;
uniform mat4 projMat;

uniform vec2 screenResolution;



in vec2 texCoord;
out vec4 fragColor;


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



float LinearizeDepth(float d,float zNear,float zFar)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}

float DepthBufferFromLinear(float zLinear, float zNear, float zFar) { 
	float z_n = (zLinear * (zFar + zNear) - 2.0 * zNear * zFar) / (zLinear * (zFar - zNear)); 
	return 0.5 * (z_n + 1.0); 
}



void main(){
	fragColor = texture(screenTexture, texCoord);


	vec2 rayCoord = texCoord * 2.0 - 1.0;

	float fov = radians(FOVdeg); // adjust as needed
	float aspect = screenResolution.x / screenResolution.y;
	float scale = tan(fov * 0.5);

	vec3 rayDir = normalize(	camForward + rayCoord.x * aspect * scale * camRight + rayCoord.y * scale * camUp	);
	vec3 rayOrigin = camPos;

	// Get the depth of the scene at the point
	float sceneDepthLinear = LinearizeDepth(texture(depthTexture, texCoord).r, camNearPlane, camFarPlane);

	vec2 intersect = raySphere(centre, oceanRadius, rayOrigin, rayDir); 
	float dstTo = intersect.x;
	float dstThrough = intersect.y;


	if (dstThrough <= 0.0)
		discard;

	// Distance from camera along the current ray
	float distanceAlongRayToScene = sceneDepthLinear / dot(rayDir, camForward);

	// Clamp dstThrough to not exceed the scene
	dstThrough = min(dstThrough, distanceAlongRayToScene - dstTo);


	if (dstThrough <= 0.0)
		discard;

	const float epsilon = 0.001;
	vec3 entryPoint = rayOrigin + (rayDir * (dstTo + epsilon));

	// coloise
	vec3 oceanColor = vec3(0.2, 0.2, 1.0);
	float alpha = clamp(dstThrough, 0, 1);
	float normal_multiplier = dot(normalize(sunPos - entryPoint), normalize(entryPoint - centre));

	fragColor.rgb = mix(fragColor.rgb, oceanColor  * normal_multiplier, alpha); 

	vec3 viewSpacePos = entryPoint - camPos;
	float camDepth = dot(rayDir, camForward);
	gl_FragDepth = DepthBufferFromLinear(camDepth, camNearPlane, camFarPlane);
	// Set the depth
} 