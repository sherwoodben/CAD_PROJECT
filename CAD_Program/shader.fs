#version 330 core

uniform int shaderType = -1;
//here, we define ALL of the valid shader types:
//DEBUG = anything less than 1 (i.e 0, -1, ...)
uniform vec4 debugColor;
//PLANE_GRID
uniform vec4 gridColor;
in vec2 gridCoordinates;
vec4 DoPlaneGrid(in vec2 gridCoords);
//SKETCH
uniform sampler2D sketchTexture;
uniform vec4 sketchObjectColor;
uniform bool isSketchObject = false;
vec4 DoSketchImage(in vec2 gridCoords, in sampler2D sketchTex);

//FLAT
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
	//SKETCH
	else if (shaderType == 2)
	{
		if (isSketchObject)
		{
			resultingColor = sketchObjectColor;
		}
		else
		{
			resultingColor = DoSketchImage(gridCoordinates, sketchTexture);
		}
	}
	//AXIS
	else if (shaderType == 3)
	{
		resultingColor = flatColor;
	}
	//POINT
	else if (shaderType == 4)
	{
		resultingColor = flatColor;
	}
	else
	{
		resultingColor = flatColor;
	}

	FragColor = resultingColor;
}
vec4 DoPlaneGrid(in vec2 gridCoords)
{
	float semiMajorDivisions = 5;
	float gridLineWidth = 0.025;
	float minorDivisions = 4.0;

	vec2 cellCoord = fract(gridCoords * semiMajorDivisions * 2.);
	vec2 minorCellCoord = fract(cellCoord * minorDivisions);
	//first, see if we are within 'x' distance of a
	//major division
	bool xMajorGrid = (cellCoord.x <= gridLineWidth / 2.) || ((1.0 - cellCoord.x) <= gridLineWidth / 2.);
	bool yMajorGrid = (cellCoord.y <= gridLineWidth / 2.) || ((1.0 - cellCoord.y) <= gridLineWidth / 2.);
	//or a minor division
	bool xMinorGrid = (minorCellCoord.x <= gridLineWidth) || ((1.0 - minorCellCoord.x) <= gridLineWidth);
	bool yMinorGrid = (minorCellCoord.y <= gridLineWidth) || ((1.0 - minorCellCoord.y) <= gridLineWidth);
	//set the color accordingly
	vec4 result = vec4(0.0f);
	if ((xMajorGrid || yMajorGrid))
	{
		result = vec4(flatColor.rgb * 0.75, 1.0);
	}
	else if ((xMinorGrid || yMinorGrid))
	{
		result = vec4(flatColor.rgb * 0.75, 0.9);
	}
	else result = vec4(0.0);

	return (result);
}

vec4 DoSketchImage(in vec2 gridCoords, in sampler2D sketchTex)
{
	vec4 returnedColor = vec4(0.0, 1.0, 1.0, 1.0);

	if ((gridCoords.x >= 0.005) && (gridCoords.x <= 0.995))
	{
		if ((gridCoords.y >= 0.005) && (gridCoords.y <= 0.995))
		{
			returnedColor = vec4(0.8, 0.8 ,0.8, 0.5);
		}
	}
	if (texture(sketchTex, gridCoords) != vec4(1.0, 1.0, 1.0, 1.0))
	{
		return texture(sketchTex, gridCoords);
	}
	else
	{
		return returnedColor;
	}
	
}

