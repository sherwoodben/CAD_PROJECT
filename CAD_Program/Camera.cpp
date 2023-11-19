#include "Camera.h"

glm::mat4 Camera::GetProjectionMatrix(float window_aspect_ratio)
{
	//if we're not in orthographic mode
	if (!this->orthographicMode)
	{
		//return the perspective matrix
		return glm::perspective(glm::radians(this->GetFOV()), window_aspect_ratio, this->GetNearClippingPlane(), this->GetFarClippingPlane());
	}
	//otherwise, we're in orthographic mode so
	else
	{
		//calculate points of the "orthographic plane" that make up the
		//left, right, top, and bottom of the screen
		float left, right, top, bottom;
		//we need to know if we're wider or taller (AR > 1 or AR < 1)
		if (window_aspect_ratio >= 1)
		{
			//top and bottom are easy in this case:
			top = -this->GetFOV() * (1);
			bottom = this->GetFOV() * (1);

			//need to adjust the left and right by the aspect ratio:
			left = -this->GetFOV() * (window_aspect_ratio);
			right = this->GetFOV() * (window_aspect_ratio);

		}
		else
		{
			//left and right are easy in this case:
			left = -this->GetFOV() * (1);
			right = this->GetFOV() * (1);

			//need to adjust the top and bottom by the aspect ratio:
			top = - this->GetFOV() / (window_aspect_ratio);
			bottom = this->GetFOV() / (window_aspect_ratio);
			
		}
		

		//then return the orthographic matrix
		return glm::ortho(left * this->cameraZoom, right * this->cameraZoom, top * this->cameraZoom, bottom * this->cameraZoom, this->GetNearClippingPlane(), this->GetFarClippingPlane());
	}
    
}

void Camera::ChangeZoom(float zoomDelta)
{
	if (!this->orthographicMode)
	{
		this->cameraZoom += zoomDelta * 0.125 * this->cameraZoom;
	}
	else
	{
		this->cameraZoom += zoomDelta * 0.025 * this->cameraZoom;
	}

	this->UpdateCamera();

}

void Camera::UpdateCamera()
{
	glm::vec3 newCameraPos = this->GetPosition();
	
	if (!this->orthographicMode)
	{
		if (this->cameraZoom > 100.0f)
		{
			this->cameraZoom = 100.0f;
		}
		else if (this->cameraZoom < 0.25f)
		{
			this->cameraZoom = 0.25f;
		}
		
		newCameraPos = glm::normalize(newCameraPos);
		newCameraPos = { newCameraPos.x * this->cameraZoom, newCameraPos.y * this->cameraZoom, newCameraPos.z * this->cameraZoom };
	}
	else
	{
		if (this->cameraZoom >1.5f)
		{
			this->cameraZoom = 1.5f;
		}
		else if (this->cameraZoom < 0.1f)
		{
			this->cameraZoom = 0.1f;
		}

		newCameraPos = glm::normalize(newCameraPos);
		newCameraPos = { newCameraPos.x * 100.0f, newCameraPos.y * 100.0f, newCameraPos.z * 100.0f };
	}

	this->SetPosition(newCameraPos);
}

void Camera::ArcBall(double deltaX, double deltaY, double angleX, double angleY)
{
	glm::vec4 camPositionHomogenous;
	if (this->orthographicMode)
	{
		camPositionHomogenous = glm::vec4(this->position, 1.0f);
	}
	else
	{
		camPositionHomogenous = glm::vec4(glm::normalize(this->position), 1.0f);
	}
	glm::vec4 pivotPositionHomogenous = glm::vec4(this->target, 1.0f);

	double cosAngle = glm::dot(this->GetViewDirection(), this->cameraUp);

	int signDAngle;
	if (angleY < 0.0f)
	{
		signDAngle = -1;
	}
	else if (angleY == 0.0f)
	{
		signDAngle = 0;
	}
	else
	{
		signDAngle = 1;
	}

	if (cosAngle * (double)signDAngle > 0.999f)
	{
		angleY = 0.0f;
	}

	glm::mat4x4 rotationMatrixX(1.0f);
	rotationMatrixX = glm::rotate(rotationMatrixX, (float)angleX, this->cameraUp);

	camPositionHomogenous = rotationMatrixX * (camPositionHomogenous - pivotPositionHomogenous) + pivotPositionHomogenous;

	glm::mat4x4 rotationMatrixY(1.0f);
	rotationMatrixY = glm::rotate(rotationMatrixY, (float)angleY, this->GetCameraRight());

	glm::vec3 finalCameraHomogenous = rotationMatrixY * (camPositionHomogenous - pivotPositionHomogenous) + pivotPositionHomogenous;

	finalCameraHomogenous = glm::vec3(finalCameraHomogenous.x * this->GetViewRadius(), finalCameraHomogenous.y * this->GetViewRadius(), finalCameraHomogenous.z * this->GetViewRadius());

	this->SetPosition(finalCameraHomogenous);

	//update the up  vector:
	//this->cameraRight = rotationMatrixY * rotationMatrixX * glm::vec4(this->cameraRight, 1.0f);
	this->cameraUp = rotationMatrixY * rotationMatrixX * glm::vec4(this->cameraUp, 1.0f);
}
