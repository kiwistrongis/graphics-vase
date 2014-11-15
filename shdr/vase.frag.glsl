#version 330 compatibility

//uniform sampler2D tex;

in vec3 normal;
in vec2 tex_coord;

vec4 base = vec4( 0.7, 0.3, 0.3, 1.0);

void main(){
	//gl_FragColor = texture( tex, tex_coord);
	//gl_FragColor.a = base.a;
	gl_FragColor = base;
}
