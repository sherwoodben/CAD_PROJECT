#version 330 core

uniform vec4 flatColor;
uniform float alpha;

out vec4 FragColor;

void main()
{
	FragColor = vec4(flatColor.rgb, alpha);
}