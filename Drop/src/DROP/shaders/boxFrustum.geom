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
layout (line_strip, max_vertices = 16) out;

uniform vec3 maxs;
uniform vec3 mins;

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

	vec3 currMax = maxs;
//	vec3 currMax = vec3(3.0, 5.0, 7.0); // Debug purpose
	vec3 currMin = mins;
//	vec3 currMin = vec3(1.0, 2.0, 3.0); // Debug purpose

	vec3 va = P.xyz + vec3(currMin.x, currMin.y, currMin.z);
	vec3 vb = P.xyz + vec3(currMax.x, currMin.y, currMin.z);
	vec3 vc = P.xyz + vec3(currMax.x, currMin.y, currMax.z);
	vec3 vd = P.xyz + vec3(currMin.x, currMin.y, currMax.z);

	vec3 ve = P.xyz + vec3(currMin.x, currMax.y, currMin.z);
	vec3 vf = P.xyz + vec3(currMax.x, currMax.y, currMin.z);
	vec3 vg = P.xyz + vec3(currMax.x, currMax.y, currMax.z);
	vec3 vh = P.xyz + vec3(currMin.x, currMax.y, currMax.z);

// Cube bottom
	// a: left-bottom 
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(va.xyz, P.w);
	Vertex_UV = vec2(0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// b: left-top
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vb.xyz, P.w);
	Vertex_UV = vec2(0.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  
  
	// c: right-top
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vc.xyz, P.w);
	Vertex_UV = vec2(1.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// d: right-bottom
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vd.xyz, P.w);
	Vertex_UV = vec2(1.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// Back to the start
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(va.xyz, P.w);
	Vertex_UV = vec2(0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

// Cube Top

	// e: left-bottom 
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(ve.xyz, P.w);
	Vertex_UV = vec2(0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// f: left-top
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vf.xyz, P.w);
	Vertex_UV = vec2(0.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  
  
	// g: right-top
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vg.xyz, P.w);
	Vertex_UV = vec2(1.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// h: right-bottom
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vh.xyz, P.w);
	Vertex_UV = vec2(1.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	// Back to the start
	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(ve.xyz, P.w);
	Vertex_UV = vec2(0.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

// Connect the faces
	
	// we are in VE

	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vh.xyz, P.w);
	Vertex_UV = vec2(1.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vd.xyz, P.w);
	Vertex_UV = vec2(1.0, 0.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vc.xyz, P.w);
	Vertex_UV = vec2(1.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vg.xyz, P.w);
	Vertex_UV = vec2(0.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vf.xyz, P.w);
	Vertex_UV = vec2(0.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  

	gl_Position = projectionMatrix * viewMatrix * modelMatrix 
		* vec4(vb.xyz, P.w);
	Vertex_UV = vec2(0.0, 1.0);
	Vertex_Color = vertex[0].color;
	EmitVertex();  


	EndPrimitive();  
}