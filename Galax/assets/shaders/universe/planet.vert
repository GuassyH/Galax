#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 localPos;
out vec3 localNorm;
out vec3 normal;
out vec3 crntPos;
out vec2 texCoord;
out vec4 vertColor;
void main(){
	mat3 normalMat = mat3(transpose(inverse(model)));

	localPos = aPos;
	localNorm = aNormal;
	
	crntPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = proj * view * vec4(crntPos, 1.0);

	texCoord = aTexCoord;

	normal = normalize(normalMat * aNormal);
	vertColor = vec4(1.0);
}
