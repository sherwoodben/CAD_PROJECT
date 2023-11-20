#include "Axis.h"

float Axis::axisVertices[] =
{
    //X Y Z (model space)	//UV Coords
        //first rect
        1.0f, 1.0f, 0.0f,		1.0f, 1.0f,	//top right
        0.0f, 1.0f, 0.0f,		0.0f, 1.0f,	//top left
        0.0f, -1.0f, 0.0f,		0.0f, 0.0f,	//bottom left
        1.0f, -1.0f, 0.0f,		1.0f, 0.0f,	//bottom right
        //second rect
        1.0f, 0.0f, 1.0f,		1.0f, 1.0f,	//top right
        0.0f, 0.0f, 1.0f,		0.0f, 1.0f,	//top left
        0.0f, 0.0f, -1.0f,		0.0f, 0.0f,	//bottom left
        1.0f, 0.0f, -1.0f,		1.0f, 0.0f	//bottom right
};

unsigned int Axis::axisIndices[] =
{
    0, 1, 2,	//first triangle
    3, 0, 2,	//second triangle
    4, 5, 6,	//third triangle
    7, 4, 6		//fourth triangle
};

Axis::Axis(glm::vec3 axisVector, glm::vec3 axisPoint)
{
    this->InitAxis();
    this->axisDirection = axisVector;
    this->axisPoint = axisPoint;
}

Axis::Axis(glm::vec3 point1, glm::vec3 point2, bool reverse)
{
    this->InitAxis();
    if (!reverse)
    {
        this->axisDirection = glm::vec3(point2 - point1);
        this->axisPoint = point2;
    }
    else
    {
        this->axisDirection = glm::vec3(point1 - point2);
        this->axisPoint = point1;
    }
}

void Axis::RenderObject()
{
    //first, bind the VAO:
    glBindVertexArray(*(this->GetObjectVAOPointer()));

    //now, draw elements (all 6 of them)
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    //unbind the VAO:
    glBindVertexArray(0);
}

glm::mat4 Axis::GetModelMatrix()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //first scale the matrix-- make it looooooong
    //and skinny (like a line)
    glm::mat4 scaleMatrix = glm::scale(modelMatrix, glm::vec3{ 2.0f, this->axisThickness, this->axisThickness });

    //rotate into place
    glm::vec3 axis = glm::normalize(glm::cross({ 1.0f, 0.0f, 0.0f }, this->axisDirection));
    float angle = glm::acos(glm::dot({ 1.0f, 0.0f, 0.0f }, this->axisDirection));
    glm::mat4 rotationMatrix = glm::rotate(modelMatrix, angle, axis);

    if ((this->axisDirection == glm::vec3{ 1.0f, 0.0f, 0.0f } || this->axisDirection == glm::vec3{ -1.0f, 0.0f, 0.0f }))
    {
        rotationMatrix = glm::mat4(1.0f);
    }

    //then, translate the matrix
    glm::mat4 translateMatrix = glm::translate(modelMatrix, this->GetObjectPosition());

    modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;


;   return modelMatrix;
}

void Axis::DeleteObject()
{
    glDeleteVertexArrays(1, this->GetObjectVAOPointer());
    glDeleteBuffers(1, this->GetObjectVBOPointer());
    glDeleteBuffers(1, this->GetObjectEBOPointer());
}

void Axis::InitAxis()
{
    //update the object type
    this->objectType = "Axis";
    //and the shader type
    this->SetShaderType(ShaderType::AXIS);

    //generate a VAO for this object:
    glGenVertexArrays(1, this->GetObjectVAOPointer());
    //generate a VBO for this object:
    glGenBuffers(1, this->GetObjectVBOPointer());
    //generate an EBO for this object:
    glGenBuffers(1, this->GetObjectEBOPointer());

    //bind the VAO
    glBindVertexArray(*(this->GetObjectVAOPointer()));

    //bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, *(this->GetObjectVBOPointer()));
    //send axis vertex data to the vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Axis::axisVertices), Axis::axisVertices, GL_STATIC_DRAW);

    //bind the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(this->GetObjectEBOPointer()));
    //send index data to the EBO:
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Axis::axisIndices), Axis::axisIndices, GL_STATIC_DRAW);

    //configure the vertex attribute pointers:
    //position:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(GL_FLOAT)));
    //enable it:
    glEnableVertexAttribArray(0);
    //uv coord:
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
    //enable it:
    glEnableVertexAttribArray(1);

    //unbind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //and the VAO
    glBindVertexArray(0);
}
