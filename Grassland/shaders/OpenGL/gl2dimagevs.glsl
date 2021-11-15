#version 330 core
layout (location = 0) in vec2 in_tex_coord;
out vec2 texCoord;
uniform vec2 vOrigin, vSize;

void main()
{
	vec2 realVertex = in_tex_coord * vSize + vOrigin;
	gl_Position = vec4(realVertex.x, realVertex.y, 0.0, 1.0);
	texCoord = in_tex_coord;
}

