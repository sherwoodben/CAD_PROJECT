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
    this->SetObjectPosition(axisPoint);
}

Axis::Axis(glm::vec3 point1, glm::vec3 point2, bool reverse)
{
    this->InitAxis();
    if (!reverse)
    {
        this->axisDirection = glm::normalize(glm::vec3(point2 - point1));
        this->SetObjectPosition(point1);
    }
    else
    {
        this->axisDirection = glm::normalize(glm::vec3(point2 - point1));
        this->SetObjectPosition(point2);
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

void Axis::PassShaderData(Shader* shader)
{
    shader->Use();
    //then update the shader
    shader->setMat4("model", this->GetModelMatrix());

    //update the flat color and alpha
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

void Axis::RenderObject(Shader* shader)
{
    this->PassShaderData(shader);
    //we have 36 indices (remove hard coded value at some point)
    this->RenderAsTriangles(NUM_AXIS_INDICES);
}

glm::mat4 Axis::GetModelMatrix()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //first scale the matrix-- make it looooooong
    //and skinny (like a line)
    glm::mat4 scaleMatrix = glm::scale(modelMatrix, glm::vec3{ 50.0f, 0.2f, .2f });
    //scaleMatrix = glm::scale(scaleMatrix, glm::vec3{ WORLD_SCALAR, WORLD_SCALAR, WORLD_SCALAR });
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    //yaw
    float yaw = glm::atan(this->axisDirection.y, this->axisDirection.x);
    rotationMatrix = glm::rotate(rotationMatrix, yaw, { 0.0f, 0.0f, 1.0f });
    //pitch
    float pitch = -glm::atan(this->axisDirection.z, sqrt(this->axisDirection.x*this->axisDirection.x + this->axisDirection.y*this->axisDirection.y));
    //std::cout << pitch << std::endl;
    rotationMatrix = glm::rotate(rotationMatrix, pitch, {0.0f, 1.0f, 0.0f});
    //roll


    //translation
    glm::mat4 translateMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(translateMatrix, this->GetObjectPosition());
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
    //vertices and 36 indices (12 triangles * three points each)
    this->SetUpBuffers(Axis::axisVertices, NUM_AXIS_VERTICES, Axis::axisIndices, NUM_AXIS_INDICES);
}
