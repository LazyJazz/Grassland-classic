#version 330 core

in vec2 out_coord;
layout(location = 0) out vec4 Color0;
uniform vec4 drawColor;

void main() {
  if (length(out_coord) > 1.0)
    discard;
  Color0 = drawColor;
}
