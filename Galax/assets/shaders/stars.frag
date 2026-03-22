#version 460 core

in vec2 texCoord;

out vec4 fragColor;
void main()
{
    float dist = length(texCoord - vec2(0.5)); // center = 0.5
    float brightness = smoothstep(0.5, 0.0, dist); // circular fade

    fragColor = vec4(vec3(brightness), 1.0);
}