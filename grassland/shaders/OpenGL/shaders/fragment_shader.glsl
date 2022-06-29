#version 330 core

in vec4 fs_color;

layout(location = 0) out vec4 render_color;

void main() {
  render_color = fs_color;
}
