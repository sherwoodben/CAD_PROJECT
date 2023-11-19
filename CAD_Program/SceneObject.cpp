#include "SceneObject.h"

glm::mat4 SceneObject::GetModelMatrix()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    //first, scale the matrix
    modelMatrix = glm::scale(modelMatrix, { this->GetObjectScale(), this->GetObjectScale(), this->GetObjectScale() });
    
    //do the angle rotations
    //roll:
    modelMatrix = glm::rotate(modelMatrix, glm::radians(this->GetObjectAngles().x), {1.0f, 0.0f, 0.0f });
    //pitch:
    modelMatrix = glm::rotate(modelMatrix, glm::radians(this->GetObjectAngles().y), { 0.0f, 1.0f, 0.0f });
    //yaw:
    modelMatrix = glm::rotate(modelMatrix, glm::radians(this->GetObjectAngles().z), { 0.0f, 0.0f, 1.0f });

    //then, translate the matrix
    modelMatrix = glm::translate(modelMatrix, this->GetObjectPosition());

    return modelMatrix;
}
