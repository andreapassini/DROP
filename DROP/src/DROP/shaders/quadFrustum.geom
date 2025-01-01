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

uniform vec2 size;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in Vertex
{
  vec4 color;
} vertex[];

out vec2 Vertex_UV;
out vec4 Vertex_Color;

void main()
{
	vec4 P = gl_in[0].gl_Position;

//	vec2 currSize = vec2(10.0, 10.0); // Debug purpose
	vec2 currSize = size;

	// a: left-bottom 
	vec2 va = P.xz + vec2(-1.0, -1.0) * currSize;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(va.x, P.y, va.y, P.w);
	Vertex_UV = vec2(0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// b: left-top
	vec2 vb = P.xz + vec2(-1.0, 1.0) * currSize;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vb.x, P.y, vb.y, P.w);
	Vertex_UV = vec2(0.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  
  
	// c: right-top
	vec2 vc = P.xz + vec2(1.0, 1.0) * currSize;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vc.x, P.y, vc.y, P.w);
	Vertex_UV = vec2(1.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// d: right-bottom
	vec2 vd = P.xz + vec2(1.0, -1.0) * currSize;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vd.x, P.y, vd.y, P.w);
	Vertex_UV = vec2(1.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// Back to the start
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(va.x, P.y, va.y, P.w);
	Vertex_UV = vec2(0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  


	EndPrimitive();  
}
