#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 proj;
uniform mat4 view;

out vec3 texCoord;

out vec4 localPos;
void main()
{
    texCoord = aPos;

    mat4 viewNoTranslation = mat4(mat3(view));

    vec4 pos = proj * viewNoTranslation * vec4(aPos, 1.0);

    localPos = pos;

    gl_Position = pos.xyww;
}