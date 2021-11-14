#version 330 core

in vec2 fragTex;
layout (location = 0) out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, fragTex);
    //FragColor = vec4(FragColor.xyz * 0.01, 1.0);
    // if (FragColor.x >= 1.0) FragColor = vec4(vec3(FragColor.x - 1.0), 1.0);
}