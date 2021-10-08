#version 330 core
in vec4 vColor;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D texture0;

void main()
{
    FragColor = vColor; //texture(texture0, texCoord);//vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 