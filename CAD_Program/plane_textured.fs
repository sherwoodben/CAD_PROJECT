#version 330 core

in vec2 texCoordinates;

out vec4 FragColor;

uniform sampler2D sketchTexture;

uniform float sketchElementAlpha;
uniform float bgAlpha;

void main()
{

	//we have texCoordinates = (x,y); (x,y) E [0,1]x[0,1]

	vec4 resultingColor = texture(sketchTexture, texCoordinates);

	//

	FragColor = resultingColor;
}