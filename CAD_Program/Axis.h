#pragma once
#include "SceneObject.h"
class Axis : public SceneObject
{
public:
	//Axis is defined by a vector and a point:
	Axis(glm::vec3 axisVector, glm::vec3 axisPoint = {0.0f, 0.0f, 0.0f});
	//Axis is defined by two points:
	Axis(glm::vec3 point1, glm::vec3 point2, bool reverse);

	//Axis defined by 'X' 'Y' or 'Z'
	Axis(const char* basisDirection);

	//destructor
	~Axis() {};

	void RenderObject();

	//gets the model matrix of the object
	glm::mat4 GetModelMatrix();

	void DeleteObject();

private:
	//axis "thickness" value
	double axisThickness = 0.005f;

	//axis direction vector:
	glm::vec3 axisDirection;

	//axis offset
	glm::vec3 axisPoint = { 0.0f, 0.0f, 0.0f };

	//vertices that define the axis for rendering
	static float axisVertices[];
	//and the indices
	static unsigned int axisIndices[];

	void InitAxis();
};

