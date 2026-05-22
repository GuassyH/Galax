#version 460 core

uniform sampler2D baseTexture;
uniform sampler2D baseDepth;

in vec3 vStarWorldPos;
in vec2 texCoord;

uniform mat4 view;
uniform mat4 proj;


vec4 GetStarBrightness(float brightness){
	vec4 result = vec4(0.0);
	
    // Arbitrary weight
    float weighted_brightness = clamp((1 - (brightness * 4)), 0, 1);

    float dist = length(texCoord - vec2(0.5)); // center = 0.5
    float starBrightness = smoothstep(0.5, 0.0, dist);

	result.rgb = vec3(starBrightness * weighted_brightness);

	return result;
}


out vec4 fragColor;
void main()
{
    // From world pos to clip pos
    vec4 clip = proj * view * vec4(vStarWorldPos, 1.0);

    // Invalid projection
    if (clip.w <= 0.0)
        discard;

    vec3 ndc = clip.xyz / clip.w;
    vec2 uv = ndc.xy * 0.5 + 0.5;

    float depth = texture(baseDepth, uv).r;

    // Check if out of bounds
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || depth < 1.0)
        discard;

    vec3 baseColor = texture(baseTexture, uv).rgb;
    fragColor = GetStarBrightness(length(baseColor.rgb)) * 0.4;

    gl_FragDepth = 0.0;
}