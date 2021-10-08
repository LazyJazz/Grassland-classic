#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 vColor;

uniform mat4 gMatrix;

void main()
{
    gl_Position = gMatrix * vec4(aPos.x, aPos.y, aPos.z, gWvalue);
    gl_Position.x = gl_Position.x * hwScale;
    vColor = vec4(aColor.xyz, 1.0); //vec4(aPos.x + 0.5, aPos.y + 0.5, 1.0, 1.0);
    
    texCoord = aTexCoord;
    
}