#version 460 core


uniform sampler2D baseTexture;
uniform sampler2D baseDepth;
uniform sampler2D starTexture;

in vec2 texCoord;
out vec4 fragColor;
void main(){
	fragColor = texture(baseTexture, texCoord);
	fragColor += texture(starTexture, texCoord);

	gl_FragDepth = texture(baseDepth, texCoord).r;
}