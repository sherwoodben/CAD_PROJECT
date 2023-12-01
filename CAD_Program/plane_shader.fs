#version 330 core

in vec2 texCoordinates;

out vec4 FragColor;

uniform vec4 flatColor;
uniform float alpha;

int numCells = 10;
int numDivisionsPerCell = 10;
float gridLineWidth = 0.025;



void main()
{
	vec4 resultingColor = vec4(0.0, 0.0, 0.0, 0.0);

	//we have gridCoordinates = (x,y); (x,y) E [0,1]x[0,1]

	vec2 cellPos = fract(texCoordinates * numCells);
	vec2 cellMinorPos = fract(cellPos * numDivisionsPerCell);

	//first, see if we are within 'x' distance of a
	//major division
	bool xMajorGrid = (cellPos.x <= gridLineWidth / 2.) || ((1.0 - cellPos.x) <= gridLineWidth / 2.);
	bool yMajorGrid = (cellPos.y <= gridLineWidth / 2.) || ((1.0 - cellPos.y) <= gridLineWidth / 2.);
	//or a minor division
	bool xMinorGrid = (cellMinorPos.x <= gridLineWidth) || ((1.0 - cellMinorPos.x) <= gridLineWidth);
	bool yMinorGrid = (cellMinorPos.y <= gridLineWidth) || ((1.0 - cellMinorPos.y) <= gridLineWidth);
	
	//set the color accordingly
	if ((xMajorGrid || yMajorGrid))
	{
		resultingColor = vec4(flatColor.rgb * 0.75, 1.0);
	}
	else if ((xMinorGrid || yMinorGrid))
	{
		resultingColor = vec4(flatColor.rgb * 0.75, 0.9);
	}

	FragColor = resultingColor;
}