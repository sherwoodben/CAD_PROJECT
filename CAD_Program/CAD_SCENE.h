#pragma once

//we need access to strings and vectors
#include <string>
#include <vector>
#include <iostream>

//include associated OpenGL files
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//include relevant scene files
#include "Message.h"
#include "SceneObject.h"
#include "Plane.h"
#include "Axis.h"
#include "Camera.h"
#include "Shader.h"

//struct to store the scene state
struct SceneState
{
	bool ShiftModifier = false;
	bool AltModifier = false;

	bool ArcBallMode = false;
	double LastArcClick[2] = { 0.0f, 0.0f };
	double ArcDragVector[2] = { 0.0f, 0.0f };

	int ScreenDimensions[2] = { 1, 1};
	float ScreenAspectRatio = 1.0f;

	double MousePos[2] = { 0.0f, 0.0f };

	bool WireFrameMode = false;
	bool DebugVisualsMode = false;

	//Maybe add the camera here? Could make
	//things more "compact"

	Camera SceneCamera = Camera();
	CameraState SavedView = CameraState();

	bool SceneUpToDate = true;

};

class CAD_SCENE
{
public:
	//constructor(s) amd destructor
	CAD_SCENE();
	CAD_SCENE(std::string loadPath);
	~CAD_SCENE();

	//Scene state information; turning this into a
	//struct now
	SceneState sceneState;
	
	Shader* GetShader() { return this->sceneShader; };
	void SetShader(Shader* shader) { this->sceneShader = shader; };

	void LoadDefaultObjects();
	void LoadDefaultPlanes();
	void LoadDefaultAxes();

	void AddSceneObject(SceneObject* objectToAdd);
	std::vector<SceneObject*> GetSceneObjects() { return this->sceneObjects; };

	void DoArcBallCam();
	void DoTranslateCam();
	void SetCameraView(CameraState::DefinedView desiredView);
	void SaveCameraView() { this->sceneState.SavedView = this->sceneState.SceneCamera.GetCameraState(); };

	bool HasChanges() { return !this->sceneState.SceneUpToDate; }

	void ProcessMessage(Message msg);

	void UpdateScene();
	void UpdateMousePosition();
	void UpdateScreenProperties();

	void RenderScene();

	void CloseScene();

private:

	//a vector to keep track of SceneObjects
	std::vector<SceneObject*> sceneObjects;

	//keep track of the scene's shader so we can
	//update variables within the shader during
	//rendering
	Shader* sceneShader = nullptr;
};

