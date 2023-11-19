#pragma once

//include necessary OpenGL files
//(mostly for matrix stuff)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//a camera is a simple thing; it should have a position; a
//projection mode (perspective or orthographic); a "world
//up" direction; a target position; a local uo direction
class Camera
{
public:
	//constructor; takes an int as a "unique ID"
	Camera(unsigned int assignedID) : ID(assignedID) {};

	//set a new target for the camera
	void SetTarget(glm::vec3 newTarget) { this->target = newTarget; };
	glm::vec3 GetTarget() { return this->target; };

	//return the projection matrix
	glm::mat4 GetProjectionMatrix(float window_aspect_ratio);

	//return the view matrix
	glm::mat4 GetViewMatrix() { return glm::lookAt(this->position, this->target, this->cameraUp); };
	glm::vec3 GetViewDirection() { return -glm::transpose(this->GetViewMatrix())[2]; };


	//perspective mode toggles/switches
	void SetPerspectiveMode() { this->orthographicMode = false; };
	void SetOrthographicMode() { this->orthographicMode = true; };
	void ToggleOrthographicMode() { this->orthographicMode = !this->orthographicMode; };
	bool GetOrthographicMode() { return this->orthographicMode; };

	//get information about the clipping planes
	float GetNearClippingPlane() { return this->nearClippingPlane; };
	float GetFarClippingPlane() { return this->farClippingPlane; };

	//get information about the camera FOV:
	float GetFOV() { return this->FOV; };
	void SetFOV(float newFOV) { this->FOV = newFOV; };
	
	//function to get the ID:
	unsigned int GetID() { return this->ID; };

	//get and set the positions
	void SetPosition(glm::vec3 newPos) { this->position = newPos; };
	glm::vec3 GetPosition() { return this->position; };

	//get and set the camera right vector
	void SetCameraRight(glm::vec3 newRight) { this->cameraRight = newRight; };
	void SetCameraUp(glm::vec3 newUp) { this->cameraUp = newUp; };
	glm::vec3 GetCameraRight() { return glm::transpose(this->GetViewMatrix())[0]; };
	glm::vec3 GetCameraUp() { return this->cameraUp; };

	void ChangeZoom(float zoomDelta);

	void UpdateCamera();

	void ArcBall(double deltaX, double deltaY, double angleX, double angleY);

	float GetViewRadius() { return this->cameraZoom; };

private:
	//ID to keep track of the camera
	unsigned int ID = -1;

	//"camera right" direction (will change)
	glm::vec3 cameraRight = { 1.0f, 0.0f, 0.0f };
	//"camera up" direction (will change)
	glm::vec3 cameraUp = { 0.0f, 0.0f, 1.0f };
	float angle = 0.0f;

	//"world up" direction (Z-Up)
	glm::vec3 worldUp = {0.0f, 0.0f, 1.0f};

	//camera's position
	glm::vec3 position = { 0.0f, 1.0f, 0.0f };

	//camera's target
	glm::vec3 target = { 0.0f, 0.0f, 0.0f };

	//keep track of the camera FOV and zoom(s)
	float FOV = 45.0f;
	float cameraZoom = 1.0f;

	//keep track of the clipping planes
	float nearClippingPlane = 0.05f;
	float farClippingPlane = 500.0f;

	//orthographic mode flag (defaults true)
	bool orthographicMode = false;
};