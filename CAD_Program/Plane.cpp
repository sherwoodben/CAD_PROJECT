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
	//update the object type
	this->objectType = "Plane";
	//and the shader type
	this->SetShaderType(ShaderType::PLANE_GRID);
	//and update object transparency
	this->SetIsTransparent(true);


	//generate a VAO for this object:
	glGenVertexArrays(1, this->GetObjectVAOPointer());
	//generate a VBO for this object
	glGenBuffers(1, this->GetObjectVBOPointer());
	//generate an EBO for this object:
	glGenBuffers(1, this->GetObjectEBOPointer());

	//first, bind the VAO (de-reference the returned pointer!):
	glBindVertexArray(*(this->GetObjectVAOPointer()));

	//now, bind the VBO (de-reference the returned pointer!):
	glBindBuffer(GL_ARRAY_BUFFER, *(this->GetObjectVBOPointer()));
	//send the plane vertex data to the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(Plane::planeVertices), Plane::planeVertices, GL_STATIC_DRAW);

	//now, bind the EBO (de-reference the returned pointer!):
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(this->GetObjectEBOPointer()));
	//send the index data to the EBO:
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Plane::planeIndices), Plane::planeIndices, GL_STATIC_DRAW);

	//next, we'll configure the vertex attribute pointer(s):
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0*sizeof(GL_FLOAT)));
	//and enable it:
	glEnableVertexAttribArray(0);
	//uv coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
	//and enable it
	glEnableVertexAttribArray(1);
	//unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//also unbind the VAO
	glBindVertexArray(0);
}

void Plane::RenderObject()
{
	//first, bind the VAO:
	glBindVertexArray(*(this->GetObjectVAOPointer()));

	//now, draw the /elements/ (of which we have 6)
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//unbind the VAO:
	glBindVertexArray(0);
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
