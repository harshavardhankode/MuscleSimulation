#version 130

in vec4 vPosition;
//in vec4 vColor;
out vec4 color;
uniform mat4 uModelViewMatrix;

void main (void) 
{
  gl_Position = uModelViewMatrix * vPosition;
  color = vec4(0,1,1,1);
}
