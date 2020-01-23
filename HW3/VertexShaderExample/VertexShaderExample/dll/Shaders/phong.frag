#version 430
in vec3 FragNormal;
in vec3 FragPos;
in vec3 lightPos;

out vec4 frag_color;

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

void light(vec3 position, vec3 norm , out vec3 ambient, out vec3 diffuse, out vec3 spec) {
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
	
	vec3 ambient, diffuse, spec;

	if (gl_FrontFacing) {
		light(FragPos, FragNormal, ambient, diffuse, spec);
	}
	else {
		light(FragPos, -FragNormal, ambient, diffuse, spec);
	}
	
	vec3 color = (ambient+diffuse+spec);
	
	frag_color = vec4( color, 1.0);
	
	
}