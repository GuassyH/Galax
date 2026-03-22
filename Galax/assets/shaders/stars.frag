#version 460 core


struct Star{
	vec4 direction;
	float pad0[3];
	float size;
};

layout(std430, binding = 0) buffer StarsBuff {
	Star stars[];
};

uniform int numStars;

uniform vec3 camForward;
uniform vec3 camRight;
uniform vec3 camUp;

uniform float FOVdeg;

uniform int windowWidth;
uniform int windowHeight;

in vec2 texCoord;

out vec4 fragColor;
void main(){
	vec2 rayCoord = texCoord * 2.0 - 1.0;

	float fov = radians(FOVdeg); // adjust as needed
	float aspect = float(windowWidth) / float(windowHeight);
	float scale = tan(fov * 0.5);

	vec3 rayDir = normalize(	camForward + rayCoord.x * aspect * scale * camRight + rayCoord.y * scale * camUp	);

	fragColor = vec4(0.0);

	// Check stars
	float maxBrightness = 0.0;

	for (int i = 0; i < numStars; i++){
		float d = dot(stars[i].direction.xyz, rayDir);

		float b = smoothstep(1.0 - stars[i].size, 1.0, d);
		b = pow(b, 20.0);

		if (b > 0.001) {
			maxBrightness = max(maxBrightness, b);
		}
	}

	fragColor = vec4(vec3(maxBrightness), 1.0);

}