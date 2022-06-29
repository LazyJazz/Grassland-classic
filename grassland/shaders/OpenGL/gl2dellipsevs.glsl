#version 330 core

layout(location = 0) in vec2 in_coord;
out vec2 out_coord;
uniform mat3 transformMat;

void main() {
  gl_Position = vec4((transformMat * vec3(in_coord, 1.0)).xy, 0.0, 1.0);
  out_coord = in_coord;
}
