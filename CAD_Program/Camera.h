#pragma once

//include necessary OpenGL files
//(mostly for matrix stuff)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//struct to store the camera state
struct CameraState
{
	int cameraID = -1;

	bool cameraIsOrthographic = false;

	glm::vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
	glm::vec3 cameraRight = { 0.0f, 1.0f, 0.0f };
	glm::vec3 cameraUp = { 0.0f, 0.0f, 1.0f };

	glm::vec3 cameraPosition = {5.0f, 0.0f, 0.0f};

	float cameraFOV = 45.0f;
	float cameraZoom = 1.0f;

	float cameraClippingPlanes[2] = { 0.05f, 500.0f };
	
};

//a camera is a simple thing; it should have a position; a
//projection mode (perspective or orthographic); a "world
//up" direction; a target position; a local uo direction
class Camera
{
public:
	//constructor; takes an int as a "unique ID"
	Camera(unsigned int assignedID) { this->cameraState.cameraID = assignedID; };

	//set a new target for the camera
	void SetTarget(glm::vec3 newTarget) { this->cameraState.cameraTarget = newTarget; };
	glm::vec3 GetTarget() { return this->cameraState.cameraTarget; };

	//return the projection matrix
	glm::mat4 GetProjectionMatrix(float window_aspect_ratio);

	//return the view matrix
	glm::mat4 GetViewMatrix() { return glm::lookAt(this->cameraState.cameraPosition, this->cameraState.cameraTarget, this->cameraState.cameraUp); };
	glm::vec3 GetViewDirection() { return -glm::transpose(this->GetViewMatrix())[2]; };


	//perspective mode toggles/switches
	void SetPerspectiveMode() { this->cameraState.cameraIsOrthographic = false; };
	void SetOrthographicMode() { this->cameraState.cameraIsOrthographic = true; };
	void ToggleOrthographicMode() { this->cameraState.cameraIsOrthographic = !this->cameraState.cameraIsOrthographic; };
	bool GetOrthographicMode() { return this->cameraState.cameraIsOrthographic; };

	//get information about the clipping planes
	float GetNearClippingPlane() { return this->cameraState.cameraClippingPlanes[0]; };
	float GetFarClippingPlane() { return this->cameraState.cameraClippingPlanes[1]; };

	//get information about the camera FOV:
	float GetFOV() { return this->cameraState.cameraFOV; };
	void SetFOV(float newFOV) { this->cameraState.cameraFOV = newFOV; };
	
	//function to get the ID:
	unsigned int GetID() { return this->cameraState.cameraID; };

	//get and set the positions
	void SetPosition(glm::vec3 newPos) { this->cameraState.cameraPosition = newPos; };
	glm::vec3 GetPosition() { return this->cameraState.cameraPosition; };

	//get and set the camera right vector
	void SetCameraRight(glm::vec3 newRight) { this->cameraState.cameraRight = newRight; };
	void SetCameraUp(glm::vec3 newUp) { this->cameraState.cameraUp = newUp; };
	glm::vec3 GetCameraRight() { return this->cameraState.cameraRight; };
	glm::vec3 GetCameraUp() { return this->cameraState.cameraUp; };

	void ChangeZoom(float zoomDelta);

	void UpdateCamera();

	void ArcBall(double angleX, double angleY);

	float GetViewRadius() { return this->cameraState.cameraZoom; };

private:
	//we need to keep track of the camera state
	CameraState cameraState;
};