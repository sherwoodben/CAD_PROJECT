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

void Point::RenderObject()
{
    this->RenderAsTriangles(NUM_POINT_INDICES);
}

glm::mat4 Point::GetModelMatrix()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //first scale the matrix-- make it looooooong
    //and skinny (like a line)
    glm::mat4 scaleMatrix = glm::scale(modelMatrix, glm::vec3{this->pointSize, this->pointSize, this->pointSize});
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
