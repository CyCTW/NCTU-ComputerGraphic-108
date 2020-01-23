#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normalvec;


out vec3 FragNormal;
out vec3 FragPos;
out vec3 lightPos;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform vec3 LightPos;


void main() {
  gl_Position = Projection * ModelView * vec4(position, 1.0);

  FragPos = vec3( ModelView * vec4(position, 1.0) );
  lightPos = vec3( ModelView * vec4(LightPos, 1.0) );

  mat3 NormalMatrix = mat3(transpose(inverse(ModelView)));
  FragNormal = normalize(NormalMatrix * normalvec);

}
