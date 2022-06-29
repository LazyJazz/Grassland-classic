#version 330 core
layout (location = 0) in vec2 in_tex_coord;
out vec2 texCoord;
uniform mat3 transMat;

void main()
{
	vec3 realVertex = transMat * vec3(in_tex_coord, 1.0);
	texCoord = in_tex_coord;
	gl_Position = vec4(realVertex.x, realVertex.y, 0.0, 1.0);
}

