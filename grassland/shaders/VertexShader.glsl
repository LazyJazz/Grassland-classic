#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec4 vColor;
out vec3 originPos;

uniform mat4 gMatrix;

void main() {
  originPos = aPos;
  gl_Position = gMatrix * vec4(aPos.xyz, 1.0);
  vColor = vec4(aColor.xyz, 1.0);  // vec4(aPos.x + 0.5, aPos.y +
                                   // 0.5, 1.0, 1.0);
}
