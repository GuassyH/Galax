#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 normal;
out vec2 texCoord;
void main(){
	mat3 normalMat = mat3(transpose(inverse(model)));

	gl_Position = proj * view * model * vec4(aPos, 1.0);
	texCoord = aTexCoord;

	normal = normalize(normalMat * aNormal);
}
