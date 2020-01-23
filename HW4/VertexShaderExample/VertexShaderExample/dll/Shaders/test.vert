#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in float life;
layout(location = 2) in vec3 squarevert;
layout(location = 3) in vec3 color;
layout(location = 4) in vec2 texcoord;

out float frag_life;
out vec2 Texcoord;
out vec3 randcolor;

uniform mat4 Projection;
uniform mat4 ModelView;

void main() {
	float siz = 2.0;
	vec3 sqpos = position + squarevert*siz ; 
	gl_Position = Projection * ModelView * vec4(sqpos, 1.0);
	Texcoord = texcoord;	
	frag_life = life;
	randcolor = color;
}