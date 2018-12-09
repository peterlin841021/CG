#version 410 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;
uniform mat4 pm;
uniform mat4 mm;

out VS_OUT
{
	vec2 texcoord;
} vs_out;


void main(void)
{
	gl_Position = mat4(1.0) * vec4(1,1,1,1)*vec4(position, 1, 1.0);	
	vs_out.texcoord = texcoord;
}