#include "Point.h"

#define NUM_POINT_VERTICES 8
#define NUM_POINT_INDICES 36

float Point::pointVertices[] =
{
    //X Y Z (model space)	//UV Coords
        //first rect
        1.0f, 1.0f, -1.0f,		1.0f, 1.0f,	//top right (BOTTOM)
        -1.0f, 1.0f, -1.0f,		0.0f, 1.0f,	//top left (BOTTOM)
        -1.0f, -1.0f, -1.0f,		0.0f, 0.0f,	//bottom left (BOTTOM)
        1.0f, -1.0f, -1.0f,		1.0f, 0.0f,	//bottom right (BOTTOM)
        //second rect
         1.0f, 1.0f, 1.0f,		1.0f, 1.0f,	//top right (TOP)
        -1.0f, 1.0f, 1.0f,		0.0f, 1.0f,	//top left (TOP)
        -1.0f, -1.0f, 1.0f,		0.0f, 0.0f,	//bottom left (TOP)
        1.0f, -1.0f, 1.0f,		1.0f, 0.0f,	//bottom right (TOP)
};

unsigned int Point::pointIndices[] =
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

void Point::PassShaderData(Shader* shader)
{
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
    //update the flat color uniform and the alpha uniform
    shader->setVec4("flatColor", flatColor);
    shader->setFloat("alpha", a);

    //std::cout << glGetError() << std::endl;
}

void Point::RenderObject(Shader* shader)
{
    this->PassShaderData(shader);
    this->RenderAsTriangles(NUM_POINT_INDICES);
}

glm::mat4 Point::GetModelMatrix()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //first scale the matrix
    glm::mat4 scaleMatrix = glm::scale(modelMatrix, glm::vec3{0.5f, 0.5f, 0.5f});
    //then, translate the matrix
    glm::mat4 translateMatrix = glm::translate(modelMatrix, this->GetObjectPosition());

    modelMatrix = translateMatrix * scaleMatrix;
    
    return modelMatrix;
}

void Point::DeleteObject()
{
    this->DeleteBuffers();
}

void Point::InitPoint()
{
    this->isDatumObject = true;

    this->objectType = "Point";

    this->SetShaderType(ShaderType::POINT);

    this->SetUpBuffers(Point::pointVertices, NUM_POINT_VERTICES, Point::pointIndices, NUM_POINT_INDICES);
}
