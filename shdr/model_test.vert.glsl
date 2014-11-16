#version 330 core

uniform mat4 model_view;
uniform mat4 projection;

in vec4 vertex_in;
in vec3 normal_in;
in vec2 texbind_in;

out vec3 normal;
out vec2 texbind;

void main(){
	gl_Position = projection * model_view * vertex_in;
	normal = ( model_view * vec4( normal_in, 1.0)).xyz;
	texbind = texbind_in;
}
