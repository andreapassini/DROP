#version 410 core

in vec4 Vertex_Color;

// output shader variable
out vec4 colorFrag;

void main (void)
{

  // colorFrag = vec4(t, 1.0) * Vertex_Color;

  colorFrag = vec4(1.0, 0.5, 0.5, 1.0);
}