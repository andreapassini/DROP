#version 410 core

// vertex position in world coordinates
// the number used for the location in the layout qualifier is the position of the vertex attribute
// as defined in the Mesh class
layout (location = 0) in vec3 position;

// model matrix
uniform mat4 modelMatrix;
// view matrix
uniform mat4 viewMatrix;

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
    // We wanna work in View coordinate to simplify the geometry shader
    gl_Position = viewMatrix * modelMatrix * vec4(position, 1.0f);

    // The classic vertex shader, left here as a reference
    // gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);

    vertex.color = colorIn;
}
