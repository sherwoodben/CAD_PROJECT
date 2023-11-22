#pragma once
#include "SceneObject.h"
class Point : public SceneObject
{
public:
	Point(glm::vec3 pointPos) { this->InitPoint(); this->SetObjectPosition(pointPos); };

	~Point() {};

	void RenderObject();

	glm::mat4 GetModelMatrix();

	void DeleteObject();

private:
	double pointSize = 0.02f;

	static float pointVertices[];
	static unsigned int pointIndices[];

	void InitPoint();
};

