#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 instancePos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float instanceSize;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 origo;

out vec3 vStarWorldPos;
out vec2 texCoord;

uniform float fov;

void main()
{
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]);


    float size = (fov / 80) * instanceSize; // Crude resize
    vec3 worldPos = instancePos + aPos.x * size * right + aPos.y * size * up;
    worldPos += origo;

    // The actual star position in world space
    vStarWorldPos = instancePos + origo;

    gl_Position = proj * view * vec4(worldPos, 1.0);

    texCoord = aTexCoords;
}