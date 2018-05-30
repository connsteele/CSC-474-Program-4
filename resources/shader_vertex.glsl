#version 410 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in int vertimat; //changed from unsigned int, that was causing crashes

out vec3 vertex_pos;

uniform mat4 Manim[200];
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;


void main() //Skeleton Vert Shader
{
	
	mat4 Ma = Manim[vertimat];
	vec4 pos = Ma*vec4(vertPos,1.0);

	//Fix the skeleton monster hands issue
	pos.x = Ma[3][0];  
    pos.y = Ma[3][1];
    pos.z = Ma[3][2];

	//swap the x and z so it walks sideways
	float temp = pos.x;
	pos.x = pos.z;
	pos.z = temp;

	gl_Position = P * V * M * pos;
	
}
