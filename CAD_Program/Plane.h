#pragma once

//since we derive from SceneObject, include that first
#include "SceneObject.h"

//we'll want the plane to be our first example of
//a SceneObject; it's even a special default one
class Plane : public SceneObject
{
public:
	//will pass an array of floats representing
	//the normal vector components
	Plane(glm::vec3 planeNormal, glm::vec3 planeTangent = {0.0f, 0.0f, 0.0f});

	//default planes defined by two character
	//representing the normal direction
	Plane(const char* basisDirection);
	~Plane() {};

	void PassShaderData(Shader* shader);
	void RenderObject(Shader* shader);

	//gets the model matrix of the object
	glm::mat4 GetModelMatrix();

	void DeleteObject();

	glm::vec3 GetNormalVector() { return this->normalVector; };
	glm::vec3 GetPlaneRightVector() { return this->tangentVector; };
	glm::vec3 GetPlaneUpVector() { return glm::cross(this->normalVector, this->tangentVector); };

	//vertices that defines the plane
	static float planeVertices[];
	//indices of vertex that make
	//two triangles (a quad)
	static unsigned int planeIndices[];

private:
	//grid details:
	//	major grid spacing given by gridSpacing
	double majorGridSpacing = 100.0f;
	//	minor divisions given by numDivisions
	unsigned int numDivisions = 4;

	//plane normal vector
	glm::vec3 normalVector;
	//plane tangent vector (defines the X-axis
	//within the plane)
	glm::vec3 tangentVector;

	void InitPlane();

};