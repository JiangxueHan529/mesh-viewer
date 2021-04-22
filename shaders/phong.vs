#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
out vec3 LightIntensity;
struct LightInfo {
 vec4 position; // Light position in eye coords.
 vec3 color;
};
uniform LightInfo uLight;
struct MaterialInfo {
 vec3 Ka; // Ambient reflectivity
 vec3 Kd; // Diffuse reflectivity
 vec3 Ks; // Specular reflectivity
 float shininess; // Specular shininess factor
};
uniform MaterialInfo uMaterial;
uniform mat4 uMV;
uniform mat3 uNMV;
uniform mat4 ProjectionMatrix;
uniform mat4 mvp;
void main()
{
 vec3 tnorm = normalize( uNMV * VertexNormal);
 vec4 eyeCoords = uMV *vec4(VertexPosition,1.0);
 vec3 s = normalize(uLight.position.xyz - vec3(eyeCoords));
 vec3 v = normalize(-eyeCoords.xyz);
 vec3 r = reflect( -s, tnorm );
 vec3 ambient = uLight.color * uMaterial.Ka;
 float sDotN = max( dot(s,tnorm), 0.0 );
 vec3 diffuse = uLight.color * uMaterial.Kd * sDotN;
 vec3 spec = vec3(0.0);
 if( sDotN > 0.0 )
 spec = uLight.color * uMaterial.Ks *pow( max( dot(r,v), 0.0 ), uMaterial.shininess );
 LightIntensity = ambient + diffuse + spec;
 gl_Position = mvp * vec4(VertexPosition,1.0);
}

