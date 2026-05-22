#version 460 core

in vec3 crntPos;
in vec2 texCoord;
in vec4 vertColor;

uniform vec4 color = vec4(1.0, 1.0, 0.7, 1.0);
out vec4 fragCol;
void main(){
	fragCol = color;
}