#version 430 core

in vec4 Vertex_Color;

// output shader variable
out vec4 colorFrag;

void main (void)
{
  // colorFrag = vec4(Vertex_Color, 1.0);
  colorFrag = Vertex_Color;
}