#version 410 core

// vertex position in world coordinates
layout (location = 0) in vec3 position;
// vertex normal in world coordinate
layout (location = 1) in vec3 normal;

// model matrix
uniform mat4 modelMatrix;
// view matrix
uniform mat4 viewMatrix;
// Projection matrix
uniform mat4 projectionMatrix;

// normals transformation matrix (= transpose of the inverse of the model-view matrix)
uniform mat3 normalMatrix;

out VS_OUT {
    vec3 normal;
} vs_out;

void main(){

  // vertex position in world coordinates
  vec4 mPosition = modelMatrix * vec4( position, 1.0 );
  // vertex position in camera coordinates
  vec4 mvPosition = viewMatrix * mPosition;

  // transformations are applied to the normal
  vs_out.normal = normalize( normalMatrix * normal );

  // we apply the projection transformation
  gl_Position = projectionMatrix * mvPosition;
}
