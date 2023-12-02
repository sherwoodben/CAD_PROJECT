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

Plane::Plane(glm::vec3 planeNormal, glm::vec3 planeTangent) : normalVector(glm::normalize(planeNormal))
{
	this->InitPlane();

	if (planeTangent != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		this->tangentVector = glm::normalize(planeTangent);
	}
	else
	{
		this->tangentVector = -glm::normalize(glm::cross(this->normalVector, { 0.0f, 0.0f, 1.0f }));
	}
}

Plane::Plane(const char* basisDirection)
{
	this->InitPlane();

	//XY Plane
	if (basisDirection == "z" || basisDirection == "Z")
	{
		this->normalVector = { 0.0f, 0.0f, 1.0f };
		this->tangentVector = { 1.0f, 0.0f, 0.0f };
		this->displayName = "X - Y Plane";
		this->isDefaultObject = true;
		this->SetDebugColor({ 255, 0, 0 });
	}
	//XZ Plane
	else if (basisDirection == "y" || basisDirection == "Y")
	{
		this->normalVector = { 0.0f, 1.0f, 0.0f };
		this->tangentVector = { -1.0f, 0.0f, 0.0f };
		this->displayName = "X - Z Plane";
		this->isDefaultObject = true;
		this->SetDebugColor({ 0, 0, 255 });
	}
	//YZ Plane
	else if (basisDirection == "x" || basisDirection == "X")
	{
		this->normalVector = { 1.0f, 0.0f, 0.0f };
		this->tangentVector = { 0.0f, 1.0f, 0.0f };
		this->displayName = "Y - Z Plane";
		this->isDefaultObject = true;
		this->SetDebugColor({ 0, 255, 0 });
	}
	else
	{
		this->displayName = "INVALID PLANE";
		this->normalVector = { 0.0f, 0.0f, 0.0f };
		this->tangentVector = { 0.0f, 0.0f, 0.0f };
		this->isDefaultObject = true;
		this->SetDebugColor({ 255, 0, 255 });
	}

	
}

void Plane::PassShaderData(Shader* shader)
{
	glBindVertexArray(*this->GetObjectVAOPointer());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	shader->Use();
	
	//then update the shader
	shader->setMat4("model", this->GetModelMatrix());

	//set the flat color
	float r, g, b, a;
	r = (float)this->GetFlatColor().r;
	g = (float)this->GetFlatColor().g;
	b = (float)this->GetFlatColor().b;
	a = (float)this->GetFlatColor().a;
	//scale them to be between 0 and 1 (divide by 255)
	r /= 255.0f;
	g /= 255.0f;
	b /= 255.0f;
	a /= 255.0f;
	//now, make  the color a vec4
	glm::vec4 flatColor = glm::vec4(r, g, b, 1.0f);
	//update the flat color uniform and alpha uniform
	shader->setVec4("flatColor", flatColor);
	shader->setFloat("alpha", a);
	//std::cout << glGetError() << std::endl;
}

void Plane::RenderObject(Shader* shader)
{
	this->PassShaderData(shader);
	//we have 6 indices making up the triangles
	this->RenderAsTriangles(6);
}

glm::mat4 Plane::GetModelMatrix()
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	////first, scale the matrix
	glm::mat4 scaleMatrix = glm::scale(modelMatrix, glm::vec3{ 50.0f, 50.0f, 50.0f });
	//scaleMatrix = glm::scale(scaleMatrix, glm::vec3{ WORLD_SCALAR, WORLD_SCALAR, WORLD_SCALAR });

	glm::vec3 surfNormal = glm::normalize(this->normalVector);
	glm::vec3 surfTangent = glm::normalize(this->tangentVector);
	glm::vec3 surfBiTangent = glm::normalize(glm::cross(surfNormal, surfTangent));

	//rotation is "just" a change of basis since
	//everything is normalized
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix[0] = { surfTangent, 0.0f };
	rotationMatrix[1] = { surfBiTangent, 0.0f };
	rotationMatrix[2] = { surfNormal, 0.0f };
	rotationMatrix[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	////then, translate the matrix
	glm::mat4 translateMatrix = glm::translate(modelMatrix, this->GetObjectPosition());

	modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;

	return modelMatrix;
}

void Plane::DeleteObject()
{
	this->DeleteBuffers();
}

void Plane::InitPlane()
{
	//first of all, this is a datum object
	this->isDatumObject = true;

	this->tangentVector = glm::cross(this->normalVector, { 0.0f, 0.0f, 1.0f });

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
