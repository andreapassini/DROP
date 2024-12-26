#version 410 core

// From: https://www.geeks3d.com/20111111/simple-introduction-to-geometry-shaders-glsl-opengl-tutorial-part1/
// The geometry shader (or GS) stage is located after the vertex shader 
// (or tessellation if used) one and before the rasterizer and pixel shader ones
// The geometry shader is the latest stage of the 3D pipeline that can manipulate geometry. 
// The geometry shader acts on a complete primitive (triangle or line):
//      it can modify existing primitives,
//      it can insert (create) new primitives,
//      it can remove (destroy) existing primitives.

layout (points) in;
layout (line_strip, max_vertices = 5) out;

// Projection matrix
uniform mat4 projectionMatrix;

in Vertex
{
  vec4 color;
} vertex[];

out vec4 Vertex_Color;

void main()
{
	vec4 P = gl_in[0].gl_Position;

	float size = 5.0;

	// a: left-bottom 
	vec2 va = P.xy + vec2(-0.5, -0.5) * size;
	// gl_Position = projectionMatrix * vec4(va, P.zw);
	gl_Position = projectionMatrix * vec4(va, 0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// b: left-top
	vec2 vb = P.xy + vec2(-0.5, 0.5) * size;
	gl_Position = projectionMatrix * vec4(vb, 0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  
  
	// d: right-bottom
	vec2 vd = P.xy + vec2(0.5, -0.5) * size;
	gl_Position = projectionMatrix * vec4(vd, 0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// c: right-top
	vec2 vc = P.xy + vec2(0.5, 0.5) * size;
	gl_Position = projectionMatrix * vec4(vc, 0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	EndPrimitive();  
}
