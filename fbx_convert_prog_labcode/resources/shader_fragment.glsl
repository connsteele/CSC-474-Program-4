#version 410 core

in vec3 vertex_pos;
out vec4 color;

uniform vec3 campos;
uniform sampler2D tex;
uniform sampler2D tex2;

void main() //Skeleton Frag Shader
{

	color = vec4(0,0,0,1);

}
