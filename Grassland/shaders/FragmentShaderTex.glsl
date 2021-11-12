#version 330 core

in vec2 fragTex;
layout (location = 0) out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    if (gl_FragCoord.x < 400)
        //FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        FragColor = texture(texture0, fragTex);
        //FragColor = vec4((vec3(FragColor.xyz) + vec3(1.0, 1.0, 1.0)) * 0.5, 1.0);
    else
        FragColor = texture(texture1, fragTex);
    // if (FragColor.x >= 1.0) FragColor = vec4(vec3(FragColor.x - 1.0), 1.0);
}