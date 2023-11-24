#version 330 core

uniform int shaderType = -1;
//here, we define ALL of the valid shader types:
//DEBUG = anything less than 1 (i.e 0, -1, ...)
uniform vec4 debugColor;
//PLANE_GRID = 1
uniform vec4 gridColor;
in vec2 gridCoordinates;
vec4 DoPlaneGrid(in vec2 gridCoords);
//FLAT = 2
uniform vec4 flatColor;


out vec4 FragColor;

void main()
{
	vec4 resultingColor = vec4(0.0);

	//DEBUG
	if (shaderType < 1)
	{
		resultingColor = debugColor;
	}
	//PLANE_GRID
	else if (shaderType == 1)
	{
		resultingColor = DoPlaneGrid(gridCoordinates);
	}
	//AXIS
	else if (shaderType == 2)
	{
		resultingColor = flatColor;
	}
	//POINT
	else if (shaderType == 3)
	{
		resultingColor = flatColor;
	}
	//FLAT
	else if (shaderType == 4)
	{
		resultingColor = flatColor;
	}

	FragColor = resultingColor;
}
vec4 DoPlaneGrid(in vec2 gridCoords)
{
	float semiMajorDivisions = 5;
	float minorDivisions = 10.0;
	float gridLineWidth = 0.1 / minorDivisions;
	

	vec2 cellCoord = fract(gridCoords * semiMajorDivisions * 2.);
	vec2 minorCellCoord = fract(cellCoord * minorDivisions);
	//first, see if we are within 'x' distance of a
	//major division
	bool xMajorGrid = (cellCoord.x <= gridLineWidth) || ((1.0 - cellCoord.x) <= gridLineWidth);
	bool yMajorGrid = (cellCoord.y <= gridLineWidth) || ((1.0 - cellCoord.y) <= gridLineWidth);
	//or a minor division
	bool xMinorGrid = (minorCellCoord.x <= gridLineWidth * minorDivisions) || ((1.0 - minorCellCoord.x) <= gridLineWidth * minorDivisions);
	bool yMinorGrid = (minorCellCoord.y <= gridLineWidth * minorDivisions) || ((1.0 - minorCellCoord.y) <= gridLineWidth * minorDivisions);
	//set the color accordingly
	vec4 result = vec4(0.0f);
	if (abs(gridCoords.y - 0.5) <= gridLineWidth / 4. && gridCoords.x > 0.5 && gridCoords.x < 0.6)
	{
		result = vec4(1.0, 0., 0., 1.0);
	}
	else if (abs(gridCoords.x - 0.5) <= gridLineWidth / 4. && gridCoords.y > 0.5 && gridCoords.y < 0.6)
	{
		result = vec4(0., 1., 0., 1.0);
	}
	else if ((xMajorGrid || yMajorGrid))
	{
		result = vec4(flatColor.rgb * 0.75, 0.5);
	}
	else if ((xMinorGrid || yMinorGrid))
	{
		result = vec4(flatColor.rgb * 0.75, 0.25);
	}
	

	else result = vec4(0.0);

	return (result);
}