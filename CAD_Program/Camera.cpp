#include "Camera.h"



glm::mat4 Camera::GetProjectionMatrix(float window_aspect_ratio)
{
	//if we're not in orthographic mode
	if (!this->GetOrthographicMode())
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
		return glm::ortho(left * this->GetViewRadius(),
			right * this->GetViewRadius(),
			top * this->GetViewRadius(),
			bottom* this->GetViewRadius(),
			this->GetNearClippingPlane(),
			this->GetFarClippingPlane());
	}
    
}

void Camera::GoToDefinedView(Camera::DefinedView desiredView, CameraState savedCamera)
{
	switch (desiredView)
	{
	case Camera::FRONT:
		this->cameraState = CameraState();
		break;
	case Camera::RIGHT:
		this->cameraState = CameraState();
		break;
	case Camera::TOP:
		this->cameraState = CameraState();
		break;
	case Camera::BACK:
		this->cameraState = CameraState();
		break;
	case Camera::LEFT:
		this->cameraState = CameraState();
		break;
	case Camera::BOTTOM:
		this->cameraState = CameraState();
		break;
	case Camera::SAVED:
		this->cameraState = savedCamera;
		break;
	default:
		this->cameraState = CameraState();
		break;
	}
}

void Camera::ChangeZoom(float zoomDelta)
{
	if (!this->GetOrthographicMode())
	{
		this->cameraState.cameraZoom += zoomDelta * 0.125 * this->GetViewRadius();
	}
	else
	{
		this->cameraState.cameraZoom += zoomDelta * 0.025 * this->GetViewRadius();
	}
}

void Camera::UpdateCamera()
{
	glm::vec3 newCameraPos = this->GetPosition();
	
	if (!this->GetOrthographicMode())
	{
		if (this->GetViewRadius() > 10.0f)
		{
			this->cameraState.cameraZoom = 10.0f;
		}
		else if (this->GetViewRadius() < 0.25f)
		{
			this->cameraState.cameraZoom = 0.25f;
		}
		
		newCameraPos = glm::normalize(newCameraPos);
		newCameraPos = { newCameraPos.x * this->GetViewRadius(), newCameraPos.y * this->GetViewRadius(), newCameraPos.z * this->GetViewRadius() };
	}
	else
	{
		if (this->GetViewRadius() >0.1f)
		{
			this->cameraState.cameraZoom = 0.1f;
		}
		else if (this->GetViewRadius() < 0.001f)
		{
			this->cameraState.cameraZoom = 0.001f;
		}

		newCameraPos = glm::normalize(newCameraPos);
		//newCameraPos = { newCameraPos.x * this->GetViewRadius(), newCameraPos.y * this->GetViewRadius(), newCameraPos.z * this->GetViewRadius() };
		newCameraPos = { newCameraPos.x * 100.0f, newCameraPos.y * 100.0f, newCameraPos.z * 100.0f };
	}

	this->SetPosition(newCameraPos);
}

void Camera::ArcBall(double angleX, double angleY)
{
	glm::vec4 camPositionHomogeneous = glm::vec4(this->GetPosition(), 1.0f);
	glm::vec4 camSightHomogeneous = glm::vec4(-this->GetPosition(), 1.0f);
	
	glm::vec4 pivotPositionHomogeneous = glm::vec4(this->GetTarget(), 1.0f);

	glm::vec4 newUpHomogeneous = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 newRightHomogeneous = glm::vec4(this->GetCameraRight(), 1.0f);

	glm::mat4x4 rotationMatrix(1.0f);

	rotationMatrix = glm::rotate(rotationMatrix, (float)(angleX), -glm::cross(glm::vec3(camPositionHomogeneous.x, camPositionHomogeneous.y, 0.0f), this->GetCameraRight()));
	rotationMatrix = glm::rotate(rotationMatrix, (float)angleY, -glm::vec3(newRightHomogeneous));
	

	camPositionHomogeneous = rotationMatrix * (camPositionHomogeneous - pivotPositionHomogeneous) + pivotPositionHomogeneous;

	glm::vec3 finalCameraHomogeneous = glm::vec3(camPositionHomogeneous.x * this->GetViewRadius(), camPositionHomogeneous.y * this->GetViewRadius(), camPositionHomogeneous.z * this->GetViewRadius());

	this->SetPosition(camPositionHomogeneous);

	//update the up  vector:

	this->SetCameraRight(rotationMatrix * glm::vec4(this->GetCameraRight(), 1.0f));
	this->SetCameraUp(rotationMatrix * glm::vec4(this->GetCameraUp(), 1.0f));
	//this->cameraUp = glm::cross(this->cameraRight, glm::vec3(pivotPositionHomogenous - camPositionHomogenous));
}
