#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 fragTex;

//uniform mat4 gMatrix;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.0);
    fragTex = aTex; //vec4(aPos.x + 0.5, aPos.y + 0.5, 1.0, 1.0);
}