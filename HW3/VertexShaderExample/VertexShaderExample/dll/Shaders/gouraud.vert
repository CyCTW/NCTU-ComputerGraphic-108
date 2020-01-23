#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normalvec;
layout(location = 2) in vec2 texcoords;


out vec3 FrontColor;
out vec3 BackColor;
out vec2 TexCoords;

struct Material{
	float Ka;
	float Kd;
	float Ks;
	float Shininess;
};
struct Light {
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

uniform Material M;
uniform Light L;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform vec3 LightPos;

void light(vec3 position, vec3 norm , vec3 lightPos, out vec3 ambient, out vec3 diffuse, out vec3 spec) {
	vec3 n = normalize(norm);
	vec3 v = normalize(-position);
	vec3 s = normalize(lightPos - position);
	vec3 r = reflect( -s, n);

	ambient = M.Ka * L.La;
	float Dotn = max ( dot(s, n), 0.0 );
	diffuse = L.Ld * M.Kd * Dotn;
	spec = L.Ls * M.Ks * pow( max( dot(r, v), 0.0 ), M.Shininess );
}

void main() {
  TexCoords = texcoords;
  gl_Position = Projection * ModelView * vec4(position, 1.0);
  
  vec3 FragNormal, FragPos, lightPos;
  FragPos = vec3( ModelView * vec4(position, 1.0) );
  lightPos = vec3( ModelView * vec4(LightPos, 1.0) );

  mat3 NormalMatrix = mat3(transpose(inverse(ModelView)));
  FragNormal = normalize(NormalMatrix * normalvec);


  vec3 ambient, diffuse, spec;

  light(FragPos, FragNormal, lightPos, ambient, diffuse, spec);
  FrontColor = (ambient + diffuse + spec);

  light(FragPos, -FragNormal, lightPos, ambient, diffuse, spec);
  BackColor = (ambient + diffuse + spec);



}
