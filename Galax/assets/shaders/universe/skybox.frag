#version 460 core

in vec3 texCoord;

uniform samplerCube skybox;
uniform sampler2D baseDepth;
uniform sampler2D baseTexture;

in vec4 localPos;

out vec4 fragColor;
void main()
{    
    // From world pos to clip pos

    // Invalid projection
    if (localPos.w <= 0.0)
        discard;

    vec3 ndc = localPos.xyz / localPos.w;
    vec2 uv = ndc.xy * 0.5 + 0.5;

    float depth = texture(baseDepth, uv).r;

    // Check if out of bounds
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || depth > 0.0)
        discard;


    vec3 baseColor = texture(baseTexture, uv).rgb;
    float weighted_brightness = clamp((1 - (length(baseColor.rgb) * 4)), 0, 1);
    fragColor = texture(skybox, texCoord) * weighted_brightness * 0.55;

    gl_FragDepth = 0.0;
}