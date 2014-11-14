#version 330 compatibility

in vec3 normal;
in vec3 tex_coord;

vec4 base = vec4( 0.7, 0.3, 0.3, 1.0);

void main(){
	gl_FragColor = base;
}
