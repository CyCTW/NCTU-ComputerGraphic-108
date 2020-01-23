#version 430
in vec2 Texcoord;

out vec4 frag_color;

uniform sampler2D ourTexture;

void main() {
	
	frag_color = texture(ourTexture, Texcoord);
	
}
