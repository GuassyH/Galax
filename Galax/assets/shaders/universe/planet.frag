#version 460 core


uniform vec3 camPos;
uniform vec3 sunDir;
uniform vec3 centre;
uniform float radius;

in vec3 normal;
in vec3 localPos;
in vec3 localNorm;
in vec3 crntPos;
in vec2 texCoord;
in vec4 vertColor;

vec3 getTriPlanarBlend(vec3 _wNorm){
	// in wNorm is the world-space normal of the fragment
	vec3 blending = abs( _wNorm );
	blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	return blending;
}


float directionalLight(){
	
	// diffuse lighting
	vec3 lightDirection = -normalize(sunDir);
	vec3 n = normalize(normal);
	float diffuse = max(dot(n, lightDirection), 0.1);

	// specular lighting
	float specularLight = 0.50;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, n);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return diffuse;
};

out vec4 fragCol;
void main(){

	float relative_dot = dot(normalize(crntPos - centre), normal);
	// fragCol = vertColor;
	vec4 color;

	if(relative_dot > 0.97){
		color = vec4(0.396, 0.58, 0.306, 1.0);
	}else{
		color = vec4(0.569, 0.498, 0.286, 1.0);
	}

	fragCol = color;
	if(distance(crntPos, centre) <= radius * 1.001)
		fragCol.rgb = vec3(0.7, 0.6, 0.2);


	fragCol *= directionalLight();
	fragCol.a = 1.0;
}