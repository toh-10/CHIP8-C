#version 330 core

in vec3 iPosition;
in vec2 vuv;
out vec2 iuv;

void main() {
  gl_Position = vec4(iPosition, 1);
  iuv = vuv;
}
