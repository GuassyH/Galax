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
		if ((length(localPos) - radius) + (colNoise * colorMaps[id].heightDither) > colorMaps[id].height) {

			if (angle >= colorMaps[id].steepness + (colNoise * colorMaps[id].steepDither) && colorMaps[id].steepness != 0.0)
				fragCol.rgb = colorMaps[id].steepCol.rgb;
			else
				fragCol.rgb = colorMaps[id].color.rgb;

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