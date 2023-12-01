#version 330 core

layout (location = 0) in vec2 pPos;

void main()
{
	gl_Position = vec4(pPos, 0.0, 1.0);
}