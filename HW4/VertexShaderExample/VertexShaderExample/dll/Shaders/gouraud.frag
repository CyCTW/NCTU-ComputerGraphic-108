#version 430

in vec3 FrontColor;
in vec3 BackColor;
in vec2 TexCoords;

out vec4 frag_color;

void main() {
	//frag_color = vec4(FrontColor, 1.0);
	
	if (gl_FrontFacing) {
		frag_color = vec4(FrontColor, 1.0);
	}
	else {
		frag_color = vec4(BackColor, 1.0);
	}
	
}