#include "Axis.h"

#define NUM_AXIS_VERTICES 8
#define NUM_AXIS_INDICES 36

float Axis::axisVertices[] =
{
    //X Y Z (model space)	//UV Coords
        //first rect
        1.0f, 1.0f, -1.0f,		1.0f, 1.0f,	//top right (BOTTOM)
        0.0f, 1.0f, -1.0f,		0.0f, 1.0f,	//top left (BOTTOM)
        0.0f, -1.0f, -1.0f,		0.0f, 0.0f,	//bottom left (BOTTOM)
        1.0f, -1.0f, -1.0f,		1.0f, 0.0f,	//bottom right (BOTTOM)
        //second rect
         1.0f, 1.0f, 1.0f,		1.0f, 1.0f,	//top right (TOP)
        0.0f, 1.0f, 1.0f,		0.0f, 1.0f,	//top left (TOP)
        0.0f, -1.0f, 1.0f,		0.0f, 0.0f,	//bottom left (TOP)
        1.0f, -1.0f, 1.0f,		1.0f, 0.0f,	//bottom right (TOP)
};

unsigned int Axis::axisIndices[] =
{
    0, 1, 2,
    3, 0, 2,
    4, 5, 6,
    7, 4, 6,
    7, 4, 0,
    3, 7, 0,
    4, 5, 1,
    0, 4, 1,
    5, 6, 2,
    1, 5, 2,
    6, 7, 2,
    3, 2, 7

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

Axis::Axis(const char* basisDirection)
{
    this->InitAxis();
    if (basisDirection == "x" || basisDirection == "X")
    {
        this->displayName = "X - Axis";
        this->axisDirection = { 1.0f, 0.0f, 0.0f };
        this->isDefaultObject = true;
        this->SetDebugColor({ 128, 0, 0 });
        this->SetFlatColor({ 255, 0, 0 });
    }
    else if (basisDirection == "y" || basisDirection == "Y")
    {
        this->displayName = "Y - Axis";
        this->axisDirection = { 0.0f, 1.0f, 0.0f };
        this->isDefaultObject = true;
        this->SetDebugColor({ 0, 128, 0 });
        this->SetFlatColor({ 0, 255, 0 });
    }
    else if (basisDirection == "z" || basisDirection == "Z")
    {
        this->displayName = "Z - Axis";
        this->axisDirection = { 0.0f, 0.0f, 1.0f };
        this->isDefaultObject = true;
        this->SetDebugColor({ 0, 0, 128 });
        this->SetFlatColor({ 0, 0, 255 });
    }
    else
    {
        this->displayName = "INVALID AXIS";
        this->axisDirection = { 0.0f, 0.0f, 0.0f };
        this->isDefaultObject = true;
        this->SetDebugColor({ 255, 0, 255 });
        this->SetFlatColor({ 255, 0, 255 });
    }
}

void Axis::RenderObject()
{
    //we have 36 indices (remove hard coded value at some point)
    this->RenderAsTriangles(NUM_AXIS_INDICES);
}

glm::mat4 Axis::GetModelMatrix()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //first scale the matrix-- make it looooooong
    //and skinny (like a line)
    glm::mat4 scaleMatrix = glm::scale(modelMatrix, glm::vec3{ 1.0f, this->axisThickness, this->axisThickness });

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


    return modelMatrix;
}

void Axis::DeleteObject()
{
    this->DeleteBuffers();
}

void Axis::InitAxis()
{
    //first of all, this is a datum object
    this->isDatumObject = true;

    //update the object type
    this->objectType = "Axis";
    //and the shader type
    this->SetShaderType(ShaderType::AXIS);

    //set up all the buffers; axes are defined with 8
    //vertices and 12 indices (four triangles * three points each)
    this->SetUpBuffers(Axis::axisVertices, NUM_AXIS_VERTICES, Axis::axisIndices, NUM_AXIS_INDICES);
}
