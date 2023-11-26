#pragma once
#include "SceneObject.h"

#include <vector>
#include "Plane.h"

struct SketchObject
{
	enum SketchObjectType
	{
		POINT,
		LINE,
		CURVE,
	} sketchObjectType = SketchObjectType::POINT;


	glm::vec2 objectOffset = glm::vec2(0.0f);
	unsigned int sketchObjectID = -1;

	unsigned int numPoints = -1;
	std::vector<glm::vec3> objectPoints;

	SketchObject()
	{
		//first add the origin to the list of points in our VBO
		this->objectPoints.push_back(glm::vec3(0.0f));
		this->numPoints = 1;

		//generate a VAO, VBO, and EBO for this object
		glGenVertexArrays(1, &this->sketchObjectVAO);
		glGenBuffers(1, &this->sketchObjectVBO);
		glGenBuffers(1, &this->sketchObjectEBO);

		//bind the VAO
		glBindVertexArray(this->sketchObjectVAO);
		//bind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->sketchObjectVBO);
		//we send data to the VBO
		float* vertexArray = new float[3 * this->numPoints];
		for (unsigned int i = 0; i < this->numPoints; i++)
		{
			vertexArray[i] = (this->objectPoints[i]).x;
			vertexArray[i+1] = (this->objectPoints[i]).y;
			vertexArray[i+2] = (this->objectPoints[i]).z;
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numPoints, vertexArray, GL_STATIC_DRAW);
		delete[] vertexArray;
		//bind the EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->sketchObjectEBO);
		//we have no data to send to the EBO at this point
		
		//configure the vertex attribute pointer. data will
		//be just pairs of x and y points, z will be 0 always.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(GL_FLOAT)));
		//now enable the vertex attrib pointer
		glEnableVertexAttribArray(0);

		//unbind the EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//unbind the VAO
		glBindVertexArray(0);
	};
	~SketchObject()
	{
		glDeleteVertexArrays(1, &this->sketchObjectVAO);
		glDeleteBuffers(1, &this->sketchObjectVBO);
		glDeleteBuffers(1, &this->sketchObjectEBO);
	};

	void Render(unsigned int renderTarget);

	unsigned int sketchObjectVAO;
	unsigned int sketchObjectVBO;
	unsigned int sketchObjectEBO;
};



class Sketch : public SceneObject
{
public:
	Sketch(Plane* parentPlane) { sketchPlane = parentPlane; this->InitSketch(); };
	~Sketch() {};

	void RenderObject();

	glm::mat4 GetModelMatrix() { return this->sketchPlane->GetModelMatrix(); };

	void RenderSketchObjects();

	void AddSketchObject(SketchObject* objectToAdd);
	void DeleteSketchObject(SketchObject* objectToDelete);

	void DeleteObject();
private:
	Plane* sketchPlane = nullptr;

	unsigned int nextObjectID = 0;
	std::vector<SketchObject*> sketchObjects;

	unsigned int sketchFBO = 0;
	unsigned int sketchRenderedTexture;

	void InitSketch();
};

