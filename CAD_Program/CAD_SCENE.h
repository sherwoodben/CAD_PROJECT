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
#include "Point.h"
#include "Axis.h"
#include "Plane.h"
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

	bool ShowDatumObjects = true;

	Camera SceneCamera = Camera();
	CameraState SavedView = CameraState();

	bool SceneUpToDate = true;

};

class CAD_APP;

class CAD_SCENE
{
public:
	//constructor(s) amd destructor
	CAD_SCENE(CAD_APP* parentApp) : parentApplication(parentApp) { this->LoadDefaultObjects(); };
	CAD_SCENE(CAD_APP* parentApp, std::string loadPath) : parentApplication(parentApp) { this->LoadDefaultObjects(); };
	~CAD_SCENE();

	//Scene state information; turning this into a
	//struct now
	SceneState sceneState;

	//get the parent app
	CAD_APP* GetParentApplication() { return this->parentApplication; };
	
	Shader* GetShader() { return this->sceneShader; };
	void SetShader(Shader* shader) { this->sceneShader = shader; };

	void LoadDefaultObjects();
	void LoadDefaultPlanes();
	void LoadDefaultAxes();

	void AddSceneObject(SceneObject* objectToAdd);
	std::vector<SceneObject*> GetSceneObjects() { return this->sceneObjects; };
	std::vector<SceneObject*> GetDatumObjects() { return this->datumObjects; };

	void DoArcBallCam();
	void DoTranslateCam();
	void SetCameraView(CameraState::DefinedView desiredView);
	void SaveCameraView() { this->sceneState.SavedView = *this->sceneState.SceneCamera.GetCameraState(); };

	bool HasChanges() { return !this->sceneState.SceneUpToDate; }

	void ProcessMessage(Message msg);
	void HandleDeleteObjectMessage(std::string objToDelete);
	void HandleNewObjectMessage(std::string typeToAdd);

	void UpdateScene();
	void UpdateMousePosition();
	void UpdateScreenProperties();

	void RenderScene();

	void CloseScene();

private:

	//a vector to keep track of SceneObjects
	std::vector<SceneObject*> sceneObjects;

	//a vector to keep track of DATUM SceneObjects
	std::vector<SceneObject*> datumObjects;

	//keep track of the scene's shader so we can
	//update variables within the shader during
	//rendering
	Shader* sceneShader = nullptr;

	//a pointer to the application
	CAD_APP* parentApplication = nullptr;
};

