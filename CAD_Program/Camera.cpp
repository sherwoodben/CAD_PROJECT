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
		return glm::ortho(left, right, top, bottom, this->GetNearClippingPlane(), this->GetFarClippingPlane());
	}
    
}

void Camera::UpdateCameraVectors()
{
	this->cameraDirection = glm::cross(this->cameraRight, this->cameraUp);
}
