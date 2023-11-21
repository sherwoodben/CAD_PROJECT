#include "Camera.h"



glm::mat4 Camera::GetProjectionMatrix(float window_aspect_ratio)
{
	//if we're not in orthographic mode
	if (!this->cameraState.cameraIsOrthographic)
	{
		//return the perspective matrix
		return glm::perspective(glm::radians(this->cameraState.cameraFOV), window_aspect_ratio, this->cameraState.cameraClippingPlanes[0], this->cameraState.cameraClippingPlanes[1]);
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
			top = -this->cameraState.cameraFOV * (1);
			bottom = this->cameraState.cameraFOV * (1);

			//need to adjust the left and right by the aspect ratio:
			left = -this->cameraState.cameraFOV * (window_aspect_ratio);
			right = this->cameraState.cameraFOV * (window_aspect_ratio);

		}
		else
		{
			//left and right are easy in this case:
			left = -this->cameraState.cameraFOV * (1);
			right = this->cameraState.cameraFOV * (1);

			//need to adjust the top and bottom by the aspect ratio:
			top = -this->cameraState.cameraFOV / (window_aspect_ratio);
			bottom = this->cameraState.cameraFOV / (window_aspect_ratio);
			
		}
		
		//then return the orthographic matrix
		return glm::ortho(left * this->cameraState.cameraZoom,
			right * this->cameraState.cameraZoom,
			top * this->cameraState.cameraZoom,
			bottom* this->cameraState.cameraZoom,
			this->cameraState.cameraClippingPlanes[0],
			this->cameraState.cameraClippingPlanes[1]);
	}
    
}

void Camera::GoToDefinedView(Camera::DefinedView desiredView, CameraState savedCamera)
{
	float currentZoom = this->cameraState.cameraZoom;
	bool currentOrtho = this->cameraState.cameraIsOrthographic;
	glm::vec3 currentTarget = this->cameraState.cameraTarget;

	CameraState desiredCameraState;
	switch (desiredView)
	{
	case Camera::FRONT:
		desiredCameraState.cameraPosition = {0.0f, -1.0f, 0.0f};
		desiredCameraState.cameraRight = { 1.0f, 0.0f, 0.0f };
		break;
	case Camera::RIGHT:
		desiredCameraState.cameraPosition = { 1.0f, 0.0f, 0.0f };
		desiredCameraState.cameraRight = { 0.0f, 1.0f, 0.0f };
		break;
	case Camera::TOP:
		desiredCameraState.cameraPosition = { 0.0f, 0.0f, 1.0f };
		desiredCameraState.cameraRight = { 1.0f, 0.0f, 0.0f };
		break;
	case Camera::ISOMETRIC:
		desiredCameraState.cameraPosition = { 1.0f, 1.0f, 1.0f };
		desiredCameraState.cameraRight = { -1.0f, 1.0f, 0.0f };
		desiredCameraState.cameraUp = { -1.0f, -1.0f, 1.0f };
		break;
	case Camera::BACK:
		desiredCameraState.cameraPosition = { 0.0f, 1.0f, 0.0f };
		desiredCameraState.cameraRight = { -1.0f, 0.0f, 0.0f };
		break;
	case Camera::LEFT:
		desiredCameraState.cameraPosition = { -1.0f, 0.0f, 0.0f };
		desiredCameraState.cameraRight = { 0.0f, -1.0f, 0.0f };
		break;
	case Camera::BOTTOM:
		desiredCameraState.cameraPosition = { 0.0f, 0.0f, -1.0f };
		desiredCameraState.cameraRight = { 1.0f, 0.0f, 0.0f };
		break;
	case Camera::SAVED:
		desiredCameraState = savedCamera;
		break;
	default:
		desiredCameraState = CameraState();
		currentZoom = 1.0f;
		currentOrtho = false;
		currentTarget = { 0.0f, 0.0f, 0.0f };
		break;
	}

	desiredCameraState.cameraZoom = currentZoom;
	desiredCameraState.cameraIsOrthographic = currentOrtho;
	desiredCameraState.cameraTarget = currentTarget;
	desiredCameraState.cameraPosition += currentTarget;

	this->cameraState = desiredCameraState;
}

void Camera::PerspectiveUpdate()
{
	glm::vec3 newCameraPos = this->cameraState.cameraPosition - this->cameraState.cameraTarget;

	if (this->cameraState.cameraZoom > 10.0f)
	{
		this->cameraState.cameraZoom = 10.0f;
	}
	else if (this->cameraState.cameraZoom < 0.25f)
	{
		this->cameraState.cameraZoom = 0.25f;
	}
	newCameraPos = glm::normalize(newCameraPos);
	newCameraPos = { newCameraPos.x * this->cameraState.cameraZoom, newCameraPos.y * this->cameraState.cameraZoom, newCameraPos.z * this->cameraState.cameraZoom };

	newCameraPos += this->cameraState.cameraTarget;
	this->cameraState.cameraPosition = newCameraPos;
	this->cameraState.cameraUp = glm::cross(glm::vec3(this->cameraState.cameraPosition - this->cameraState.cameraTarget), this->cameraState.cameraRight);
}

void Camera::OrthographicUpdate()
{
	glm::vec3 newCameraPos = this->cameraState.cameraPosition - this->cameraState.cameraTarget;

	if (this->cameraState.cameraZoom > 0.05f)
	{
		this->cameraState.cameraZoom = 0.05f;
	}
	else if (this->cameraState.cameraZoom < 0.001f)
	{
		this->cameraState.cameraZoom = 0.001f;
	}

	newCameraPos = glm::normalize(newCameraPos);
	newCameraPos = { newCameraPos.x , newCameraPos.y , newCameraPos.z  };

	newCameraPos += this->cameraState.cameraTarget;
	this->cameraState.cameraPosition = newCameraPos;
	this->cameraState.cameraUp = glm::cross(glm::vec3(this->cameraState.cameraPosition - this->cameraState.cameraTarget), this->cameraState.cameraRight);
}

void Camera::TranslateCam(double deltaX, double deltaY)
{
	//move by x along the camera right
	this->cameraState.cameraTarget += glm::vec3(deltaX * this->cameraState.cameraRight.x, deltaX * this->cameraState.cameraRight.y, deltaX * this->cameraState.cameraRight.z);
	this->cameraState.cameraPosition += glm::vec3(deltaX * this->cameraState.cameraRight.x, deltaX * this->cameraState.cameraRight.y, deltaX * this->cameraState.cameraRight.z);
	//move by y along the camera up
	this->cameraState.cameraTarget += glm::vec3(deltaY * this->cameraState.cameraUp.x, deltaY * this->cameraState.cameraUp.y, deltaY * this->cameraState.cameraUp.z);
	this->cameraState.cameraPosition += glm::vec3(deltaY * this->cameraState.cameraUp.x, deltaY * this->cameraState.cameraUp.y, deltaY * this->cameraState.cameraUp.z);
}

void Camera::ChangeZoom(float zoomDelta)
{
	this->cameraState.cameraZoom += zoomDelta * 0.125 * this->cameraState.cameraZoom;
}

void Camera::UpdateCamera()
{	
	if (!this->cameraState.cameraIsOrthographic)
	{
		this->PerspectiveUpdate();
		
		
	}
	else
	{
		this->OrthographicUpdate();
		
	}
	
}

void Camera::ArcBall(double angleX, double angleY)
{
	glm::vec4 camPositionHomogeneous = glm::vec4(this->cameraState.cameraPosition, 1.0f);
	glm::vec4 camSightHomogeneous = glm::vec4(-this->cameraState.cameraPosition, 1.0f);
	
	glm::vec4 pivotPositionHomogeneous = glm::vec4(this->cameraState.cameraTarget, 1.0f);

	glm::vec4 newUpHomogeneous = glm::vec4(this->cameraState.cameraUp, 1.0f);
	glm::vec4 newRightHomogeneous = glm::vec4(this->cameraState.cameraRight, 1.0f);

	glm::mat4x4 rotationMatrix(1.0f);

	glm::vec3 crossProd = glm::cross(glm::vec3(camPositionHomogeneous.x, camPositionHomogeneous.y, 0.0f), this->cameraState.cameraRight);
	if (crossProd == glm::vec3(0.0f))
	{
		crossProd = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	rotationMatrix = glm::rotate(rotationMatrix, (float)(angleX), -crossProd);
	rotationMatrix = glm::rotate(rotationMatrix, (float)angleY, -glm::vec3(newRightHomogeneous));
	
	camPositionHomogeneous = rotationMatrix * (camPositionHomogeneous - pivotPositionHomogeneous) + pivotPositionHomogeneous;

	glm::vec3 finalCameraHomogeneous = glm::vec3(camPositionHomogeneous.x * this->cameraState.cameraZoom, camPositionHomogeneous.y * this->cameraState.cameraZoom, camPositionHomogeneous.z * this->cameraState.cameraZoom);

	this->cameraState.cameraPosition = camPositionHomogeneous;

	//update the camera right vector:
	this->cameraState.cameraRight = rotationMatrix * glm::vec4(this->cameraState.cameraRight, 1.0f);
}
