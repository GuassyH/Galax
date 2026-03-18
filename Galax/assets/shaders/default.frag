#version 460 core

in vec3 normal;

in vec2 texCoord;
out vec4 fragCol;
void main(){
	// fragCol = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
	fragCol = vec4(0.5, 1.0, 0.0, 1.0);
	fragCol *= dot(normalize(vec3(1.0, 0.0, 1.0)), normal);
}