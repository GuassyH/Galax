#version 460 core

in vec3 crntPos;
in vec2 texCoord;
in vec4 vertColor;

out vec4 fragCol;
void main(){
	fragCol = vertColor;
}