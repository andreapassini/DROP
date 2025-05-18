#version 430 core

uniform sampler2D tex_0;
uniform float alphaCutOut;

in vec2 Vertex_UV;
in vec4 Vertex_Color;

// output shader variable
out vec4 colorFrag;

void main (void)
{
  vec2 uv = Vertex_UV.xy;
  uv.y *= -1.0;
  vec4 t = texture(tex_0,uv).rgba;

  colorFrag = t * Vertex_Color;

  if(colorFrag.a < alphaCutOut){
//	// Debug purpose
//	colorFrag.r = alphaCutOut;
//	colorFrag.g = 0.0;
//	colorFrag.b = 0.0;
//	colorFrag.a = 1.0;
//	// ----

	discard;
  }
}