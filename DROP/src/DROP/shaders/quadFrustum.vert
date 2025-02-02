#version 430 core

// vertex position in world coordinates
// the number used for the location in the layout qualifier is the position of the vertex attribute
// as defined in the Mesh class
layout (location = 0) in vec3 position;

// model matrix
// uniform mat4 modelMatrix;
// view matrix
// uniform mat4 viewMatrix;

// Projection matrix
// uniform mat4 projectionMatrix;

uniform vec4 colorIn;

out Vertex
{
  vec4 color;
} vertex;

void main()
{
    // transformations are applied to each vertex
    // We wanna work in Local coordinate to simplify the geometry shader
    gl_Position = vec4(position, 1.0f);

    vertex.color = colorIn;
}
