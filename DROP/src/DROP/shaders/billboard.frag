#version 430 core

uniform sampler2D tex0;
in vec2 Vertex_UV;
in vec4 Vertex_Color;

// output shader variable
out vec4 colorFrag;

void main (void)
{
  vec2 uv = Vertex_UV.xy;
  uv.y *= -1.0;
  vec3 t = texture(tex0,uv).rgb;

  // colorFrag = vec4(1.0, 0.5, 0.5, 1.0);
  colorFrag = vec4(t, 1.0) * Vertex_Color;
}