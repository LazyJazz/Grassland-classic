#version 330 core

layout(location = 0) out vec4 Color0;

uniform vec4 drawColor;

void main() {
  Color0 = drawColor;
}
