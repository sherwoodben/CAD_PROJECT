#pragma once

//include necessary OpenGL files
//(mostly for matrix stuff)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//struct to store the camera state
struct CameraState
{
	enum DefinedView
	{
		FRONT,
		RIGHT,
		TOP,
		ISOMETRIC,
		BACK,
		LEFT,
		BOTTOM,
		RESET,
		SAVED
	};

	bool cameraIsOrthographic = false;

	glm::vec3 cameraTarget = { 0.0f, 0.0f, 0.0f };
	glm::vec3 cameraRight = { 1.0f, 0.0f, 0.0f };
	glm::vec3 cameraUp = { 0.0f, 0.0f, 1.0f };

	glm::vec3 cameraPosition = { 0.0f, -5.0f, 0.0f };

	float cameraFOV = 45.0f;
	float cameraZoom = 1.0f;

	float cameraClippingPlanes[2] = { 0.005f, 500.0f };
};

//a camera is a simple thing; it should have a position; a
//projection mode (perspective or orthographic); a "world
//up" direction; a target position; a local uo direction
class Camera
{
public:

	//constructor;
	Camera() {};

	//set a new target for the camera
	void SetTarget(glm::vec3 newTarget) { this->cameraState.cameraTarget = newTarget; };
	glm::vec3 GetTarget() { return this->cameraState.cameraTarget; };

	//return the projection matrix
	glm::mat4 GetProjectionMatrix(float window_aspect_ratio);

	//return the view matrix
	glm::mat4 GetViewMatrix() { return glm::lookAt(this->GetCameraState()->cameraPosition, this->GetCameraState()->cameraTarget, this->GetCameraState()->cameraUp); };
	glm::vec3 GetViewDirection() { return -glm::transpose(this->GetViewMatrix())[2]; };

	//set the camera to a predefined view
	void GoToDefinedView(CameraState::DefinedView desiredView, CameraState savedCamera = CameraState());


	//perspective mode toggles/switches
	void SetPerspectiveMode() { this->cameraState.cameraIsOrthographic = false; };
	void SetOrthographicMode() { this->cameraState.cameraIsOrthographic = true; };

	//handle changing the zoom
	void ChangeZoom(float zoomDelta);

	//update the camera
	void UpdateCamera();
	//perspective update
	void PerspectiveUpdate();
	//orthographic update
	void OrthographicUpdate();
	//update the position(s)
	void UpdatePosition();

	//for moving the camera around in space
	void ArcBall(double angleX, double angleY);
	void TranslateCam(double deltaX, double deltaY);

	//"exposes" the camera state
	CameraState* GetCameraState() { return &this->cameraState; };

private:
	//we need to keep track of the camera state
	CameraState cameraState;
};