#version 330 core

layout(location = 0) in vec2 vs_in;

void main() {
  gl_Position = vec4(vs_in, 0.0, 1.0);
}
