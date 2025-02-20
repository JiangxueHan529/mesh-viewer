#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
out vec3 Position;
out vec3 Normal;
uniform mat4 uMV;
uniform mat3 uNMV;
uniform mat4 ProjectionMatrix;
uniform mat4 mvp;
void main()
{
 Normal = normalize( uNMV* VertexNormal);
 Position = vec3( uMV *vec4(VertexPosition,1.0) );
 gl_Position = mvp * vec4(VertexPosition,1.0);
}