#version 460 core


uniform vec3 camPos;
uniform vec3 sunDir;
uniform vec3 centre;

in vec3 normal;
in vec3 crntPos;
in vec2 texCoord;
in vec4 vertColor;

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
	fragCol *= directionalLight();
}