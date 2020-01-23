#version 430

in float frag_life;
in vec2 Texcoord;
in vec3 randcolor;

out vec4 outColor;

uniform bool switchcolor;
uniform bool _texture_;
uniform bool _fade_;
uniform bool _spcolor_;
uniform float transparent;
uniform sampler2D ourTexture;


void main() {
	//outColor = vec4(1.0, 1.0, 0.0, 1.0);
	
	if (_texture_){
		outColor = texture(ourTexture, Texcoord);
		return;
	}
	else if(_fade_) {
		outColor = vec4(1.0, 1.0, 1.0, transparent);
		return;
	}
	//else if (frag_life > 0.8) {
	//}
	else if (_spcolor_) {
		outColor = vec4(1.0, 0.0, 0.0, 0.8);
	}
	else {
		//outColor = vec4(1.0, 0.0, 0.0, 1.0);
		outColor = vec4(randcolor, 1.0);
	}
	
	
}