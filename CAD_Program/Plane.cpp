#include "Plane.h"

float Plane::planeVertices[] = 
{
		//X Y Z (model space)	//UV Coords
		1.0f, 1.0f, 0.0f,		1.0f, 1.0f,	//top right
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,	//top left
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,	//bottom left
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f	//bottom right
};


unsigned int Plane::planeIndices[] =
{
	0, 1, 2,	//first triangle
	3, 0, 2		//second triangle
};

Plane::Plane(glm::vec3 planeNormal) : normalVector(glm::normalize(planeNormal))
{
	this->InitPlane();
}

Plane::Plane(const char* basisDirection)
{
	this->InitPlane();

	//XY Plane
	if (basisDirection == "z" || basisDirection == "Z")
	{
		this->normalVector = { 0.0f, 0.0f, 1.0f };
		this->displayName = "X - Y Plane";
		this->isDefaultObject = true;
		this->SetDebugColor({ 255, 0, 0 });
	}
	//XZ Plane
	else if (basisDirection == "y" || basisDirection == "Y")
	{
		this->normalVector = { 0.0f, -1.0f, 0.0f };
		this->displayName = "X - Z Plane";
		this->isDefaultObject = true;
		this->SetDebugColor({ 0, 0, 255 });
	}
	//YZ Plane
	else if (basisDirection == "x" || basisDirection == "X")
	{
		this->normalVector = { 1.0f, 0.0f, 0.0f };
		this->displayName = "Y - Z Plane";
		this->isDefaultObject = true;
		this->SetDebugColor({ 0, 255, 0 });
	}
	else
	{
		this->displayName = "INVALID PLANE";
		this->normalVector = { 0.0f, 0.0f, 0.0f };
		this->isDefaultObject = true;
		this->SetDebugColor({ 255, 0, 255 });
	}
}

void Plane::RenderObject()
{
	//we have 6 indices making up the triangles
	this->RenderAsTriangles(6);
}

glm::mat4 Plane::GetModelMatrix()
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	//first, scale the matrix
	glm::mat4 scaleMatrix = glm::scale(modelMatrix, glm::vec3{this->GetObjectScale()});

	glm::vec3 axis = glm::normalize(glm::cross({ 0.0f, 0.0f, 1.0f }, this->normalVector));
	float angle = glm::acos(glm::dot({ 0.0f, 0.0f, 1.0f }, this->normalVector));
	glm::mat4 rotationMatrix = glm::rotate(modelMatrix, angle, axis);
	

	if ((this->normalVector == glm::vec3{ 0.0f, 0.0f, 1.0f } || this->normalVector == glm::vec3{ 0.0f, 0.0f, -1.0f }))
	{
		rotationMatrix = glm::mat4(1.0f);
	}

	//then, translate the matrix
	glm::mat4 translateMatrix = glm::translate(modelMatrix, this->GetObjectPosition());

	modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;

	return modelMatrix;
}

void Plane::DeleteObject()
{
	glDeleteVertexArrays(1, this->GetObjectVAOPointer());
	glDeleteBuffers(1, this->GetObjectVBOPointer());
	glDeleteBuffers(1, this->GetObjectEBOPointer());
}

void Plane::InitPlane()
{
	//update the object type
	this->objectType = "Plane";
	//and the shader type
	this->SetShaderType(ShaderType::PLANE_GRID);
	//and update object transparency
	this->SetIsTransparent(true);

	//set up all the buffers; planes are defined with 4
	//vertices and 6 indices (two triangles * three points each)
	this->SetUpBuffers(Plane::planeVertices, 4, Plane::planeIndices, 6);
}
