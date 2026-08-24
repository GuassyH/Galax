#version 460 core


uniform vec3 camPos;
uniform vec3 sunDir;
uniform vec3 centre;
uniform float radius;

uniform bool lit;

in vec3 normal;
in vec3 localPos;
in vec3 localNorm;
in vec3 crntPos;
in vec2 texCoord;
in vec4 vertColor;
in float colNoise;

uniform int numColorMaps;

//  64 bytes - layout matches C++ `Vertex` (vec3 + pad, vec3 + pad, vec4, vec2, pad[2])
struct ColorMap {
	vec4 color;
	vec4 steepCol;
	float height;
	float heightDither;
	float heightSharpness;
	float steepness; // match CPU padding
	float steepDither;
	float steepSharpness;
	float pad[2];
};

layout(std430, binding = 0) buffer ColorMaps {
    ColorMap colorMaps[];
};


float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
vec3 hash(vec3 p) {   return fract(sin(vec3(dot(p, vec3(1.0, 57.0, 113.0)), dot(p, vec3(57.0, 113.0, 1.0)), dot(p, vec3(113.0, 1.0, 57.0)))) * 43758.5453); }

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


/////////////////////////////////////
// Tri Planar
/////////////////////////////////////


vec3 getTriPlanarBlend(vec3 _wNorm){
	// in wNorm is the world-space normal of the fragment
	vec3 blending = abs( _wNorm );
	blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	return blending;
}



/////////////////////////////////////
// Lighting
/////////////////////////////////////



float directionalLight(){
	// diffuse lighting
	vec3 lightDirection = -normalize(sunDir);
	vec3 n = normalize(normal);
	float diffuse = max(dot(n, lightDirection), 0.0);

	// specular lighting
	float specularLight = 0.50;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, n);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return diffuse;
};




vec3 GetColor(int id, float angle, float height){
	vec3 baseCol;
	vec3 newCol;

	// If it is a "Steep" pixel
	bool isSteep = angle >= colorMaps[id].steepness + (colNoise * colorMaps[id].steepDither) && colorMaps[id].steepness != 0.0;

	if (isSteep)
		baseCol = colorMaps[id].steepCol.rgb;
	else 
		baseCol = colorMaps[id].color.rgb;

	newCol = baseCol;

	if (id > 0){
		vec3 baseMixCol = mix(colorMaps[id - 1].color.rgb, baseCol, clamp(abs(colorMaps[id].height - height) * colorMaps[id].heightSharpness, 0, 1));
	
		if (isSteep) 
			newCol = mix(colorMaps[id].color.rgb, baseMixCol, clamp(abs(colorMaps[id].height - height) * colorMaps[id].steepSharpness, 0, 1));
		else
			newCol = baseMixCol;
	}
	
	newCol *= 1 + (PerlinNoise(localPos * 0.9) / 20.0 + PerlinNoise(localPos * 1.5) / 60.0);

	return newCol;
}

/////////////////////////////////////
// Main
/////////////////////////////////////


out vec4 fragCol;
void main(){
	if (numColorMaps == 0){
		fragCol.rgb = vec3(0.0, 0.0, 0.0);
		return;
	}

	float angle = acos(dot(normalize(localPos), normal));

	// Add mix??
	bool foundHeight = false;
	for (int i = 0; i < numColorMaps; i++){
		int id = (numColorMaps - 1) - i;
		float height = (length(localPos) - radius) + (colNoise * colorMaps[id].heightDither);

		if (height > colorMaps[id].height) {

			fragCol.rgb = GetColor(id, angle, height);
	
			foundHeight = true;
			break;
		}
	}

	if (!foundHeight) {
		fragCol.rgb = colorMaps[0].color.rgb;
	}


	if(lit)
		fragCol *= directionalLight();
	
	fragCol.a = 1.0;

}