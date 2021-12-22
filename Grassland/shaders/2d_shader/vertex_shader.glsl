#version 420 core

layout (location = 0) in vec4 vs_pos;
layout (location = 1) in vec4 vs_norm;
layout (location = 2) in vec4 vs_tex;

layout (std140, binding = 0) uniform ConstantBuffer
{
	mat4 worldMat;
	mat4 projectionMat;
	int mode;
};

out vec4 fs_norm;
out vec4 fs_texcoord;

void main()
{
	gl_Position = projectionMat * worldMat * vs_pos;
	fs_norm = worldMat * vs_norm;
	fs_texcoord = vs_tex;
}
