#include "SceneObject.h"
#include "Shader.h"

void SceneObject::RenderAsTriangles(unsigned int numIndices)
{
	//first, bind the VAO:
	glBindVertexArray(this->VAO);
	
	//now, draw elements (all of them)
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

	//unbind the VAO:
	glBindVertexArray(0);
}

glm::mat4 SceneObject::GetModelMatrix()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //first, scale the matrix
    modelMatrix = glm::scale(modelMatrix, { this->objectScale, this->objectScale, this->objectScale });
    //do the angle rotations
    //roll:
    modelMatrix = glm::rotate(modelMatrix, glm::radians(this->objectAngles.x), {1.0f, 0.0f, 0.0f });
    //pitch:
    modelMatrix = glm::rotate(modelMatrix, glm::radians(this->objectAngles.y), { 0.0f, 1.0f, 0.0f });
    //yaw:
    modelMatrix = glm::rotate(modelMatrix, glm::radians(this->objectAngles.z), { 0.0f, 0.0f, 1.0f });

    //then, translate the matrix
    modelMatrix = glm::translate(modelMatrix, this->objectPosition);

    return modelMatrix;
}

void SceneObject::DeleteBuffers()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void SceneObject::SetUpBuffers(float* vertexArray, unsigned int numVertices, unsigned int* indexArray, unsigned int numIndices)
{
	//generate a VAO for this object:
	glGenVertexArrays(1, &this->VAO);
	//generate a VBO for this object
	glGenBuffers(1, &this->VBO);
	//generate an EBO for this object:
	glGenBuffers(1, &this->EBO);

	//first, bind the VAO (de-reference the returned pointer!):
	glBindVertexArray(this->VAO);

	//now, bind the VBO (de-reference the returned pointer!):
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	//send the vertex data to the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * numVertices, vertexArray, GL_STATIC_DRAW);

	//now, bind the EBO (de-reference the returned pointer!):
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	//send the index data to the EBO:
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * numIndices, indexArray, GL_STATIC_DRAW);

	//next, we'll configure the vertex attribute pointer(s):
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(GL_FLOAT)));
	//and enable it:
	glEnableVertexAttribArray(0);
	//uv coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
	//and enable it
	glEnableVertexAttribArray(1);
	//DONT UNBIND THE EBO-- IT BREAKS LITERALLY EVERYTHING
	//unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//also unbind the VAO
	glBindVertexArray(0);
}

