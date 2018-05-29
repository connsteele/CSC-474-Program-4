#version 330 core

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
out vec4 color;

uniform vec3 campos;
uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
	vec4 tcol = texture(tex, vertex_tex);
	color = tcol;
	//color.gb*=0.4;
}
