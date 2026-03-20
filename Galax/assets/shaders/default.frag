#version 460 core


uniform vec3 camPos;

in vec3 normal;
in vec3 crntPos;
in vec2 texCoord;
in vec4 vertColor;

float directionalLight(vec3 lightDir){
	
	// easy to understand
	vec3 lightDirection = normalize(lightDir);
	
	// diffuse lighting
	lightDirection = normalize(lightDirection);
	vec3 n = normalize(normal);
	float diffuse = max(dot(n, lightDirection), 0.2f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, n);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return diffuse;
};


out vec4 fragCol;
void main(){
	fragCol = vertColor;
	fragCol *= directionalLight(vec3(0.0, 0.0, 1.0));
}