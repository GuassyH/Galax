#version 460 core


uniform sampler2D depthTexture;
uniform sampler2D screenTexture;
uniform sampler2D normalTexture;

uniform float normalRepeat;
uniform float normalScale;
uniform bool hasNormalTex;

uniform float oceanRadius;
uniform float densityFalloff;
uniform vec4 oceanColor;

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


vec3 getTriPlanarBlend(vec3 _wNorm){
	// in wNorm is the world-space normal of the fragment
	vec3 blending =  max(abs(_wNorm), 0.00001);
	blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
	blending /= (blending.x + blending.y + blending.z);
	return blending;
}


vec3 normalCalculation(vec3 localPos){

	vec3 blending = getTriPlanarBlend(normalize(localPos));
	vec3 spherePos = normalize(localPos); // [-1,1] range

	vec3 xaxis = texture(normalTexture, spherePos.yz * normalRepeat).rgb * 2.0 - 1.0;
	vec3 yaxis = texture(normalTexture, spherePos.xz * normalRepeat).rgb * 2.0 - 1.0;
	vec3 zaxis = texture(normalTexture, spherePos.xy * normalRepeat).rgb * 2.0 - 1.0;

	// Reorient normals into world/object space
	vec3 nx = vec3(xaxis.z, xaxis.y, -xaxis.x);
	vec3 ny = vec3(yaxis.x, yaxis.z, -yaxis.y);
	vec3 nz = vec3(zaxis.x, zaxis.y, zaxis.z);

	vec3 normalTex = nx * blending.x + ny * blending.y + nz * blending.z;

	normalTex.xy *= normalScale;
	normalTex = normalize( normalTex );

	vec3 N = spherePos;

	// Pick a safe up vector
	vec3 up = abs(N.y) < 0.999 ? vec3(0,1,0) : vec3(1,0,0);

	vec3 T = normalize(cross(up, N));
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * normalTex);
}


float directionalLight(vec3 normal, vec3 sunDir, vec3 crntPos){
	
	// diffuse lighting
	vec3 lightDirection = -normalize(sunDir);
	vec3 n = normalize(normal);
	float diffuse = max(dot(n, lightDirection), 0.1);

	// specular lighting
	float specularLight = 0.50;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, n);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 16);
	float specular = specAmount * specularLight;

	return diffuse + specular;
};



// TAKEN FROM THE ATMOSPHERE SHADER
vec2 raySphere (vec3 sphereCentre, vec3 rayOrigin, vec3 rayDir) {
    vec3 offset = rayOrigin - sphereCentre;
    const float a = 1; // set to dot(rayDir, rayDir) if rayDir might be unnormalized
    float b = 2 * dot (offset, rayDir);
    float c = dot (offset, offset) - oceanRadius * oceanRadius;

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
float densityAtPoint(vec3 samplePoint){
	float heightAbove = distance(samplePoint, centre);
	float height01 = heightAbove / oceanRadius;
	height01 = clamp(height01, 0.0, 1.0);

	float localDensity = exp(-height01 * densityFalloff) * (1 - height01);

	return localDensity;
}


// Get depth from OpenGL depth
float LinearizeDepth(float d,float zNear,float zFar)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}

// Get OpenGL depth from linear
float DepthBufferFromLinear(float zLinear, float zNear, float zFar) { 
	float z_n = (zLinear * (zFar + zNear) - 2.0 * zNear * zFar) / (zLinear * (zFar - zNear)); 
	return 0.5 * (z_n + 1.0); 
}


void main(){
	fragColor = texture(screenTexture, texCoord);
	gl_FragDepth = texture(depthTexture, texCoord).r;

	vec2 rayCoord = texCoord * 2.0 - 1.0;

	float fov = radians(FOVdeg); // adjust as needed
	float aspect = screenResolution.x / screenResolution.y;
	float scale = tan(fov * 0.5);

	vec3 rayDir = normalize(	camForward + rayCoord.x * aspect * scale * camRight + rayCoord.y * scale * camUp	);
	vec3 rayOrigin = camPos;

	// Get the depth of the scene at the point
	float sceneDepthLinear = LinearizeDepth(texture(depthTexture, texCoord).r, camNearPlane, camFarPlane);

	vec2 intersect = raySphere(centre, rayOrigin, rayDir); 
	float dstTo = intersect.x;
	float dstThrough = intersect.y;


	if (dstThrough <= 0.0)
		return;
	

	// Distance from camera along the current ray
	float distanceAlongRayToScene = sceneDepthLinear / dot(rayDir, camForward);

	// Clamp dstThrough to not exceed the scene
	dstThrough = min(dstThrough, distanceAlongRayToScene - dstTo);


	// ACTUAL CALCS
	if (dstThrough <= 0.0)
		return;

	const float epsilon = 0.001;
	vec3 entryPoint = rayOrigin + (rayDir * (dstTo + epsilon*2));

	// colorise
	float alpha = clamp(dstThrough, 0.1, 1.0);


	if(dstThrough < 0.1)
		fragColor.rgb = vec3(1.0);
	else {
		vec3 normal;
		if (hasNormalTex && dstTo > 0.0) {
			normal = normalCalculation(entryPoint - centre) * 0.5 + 0.5;
		}else {
			normal = normalize(entryPoint - centre);
		}

		fragColor.rgb = mix(fragColor.rgb,  oceanColor.rgb, alpha);
		fragColor.rgb *= directionalLight(normal, normalize(sunPos - centre), entryPoint);
	}

	// Set the depth DONT TOUCH!!!!
	vec3 viewSpacePos = entryPoint - camPos;
	float camDepth = dot(viewSpacePos, camForward);
	gl_FragDepth = DepthBufferFromLinear(max(camDepth, 0), camNearPlane, camFarPlane);
} 