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
#include "Camera.h"
#include "Shader.h"

class CAD_SCENE
{
public:
	CAD_SCENE();
	CAD_SCENE(std::string loadPath);
	~CAD_SCENE();

	bool viewArcBallOn = false;
	double arcClickLastPos[2];
	double arcDragVector[2];

	int screenWidth, screenHeight;

	double mousePos[2] = {0, 0};
	

	
	Shader* GetShader() { return this->sceneShader; };
	void SetShader(Shader* shader) { this->sceneShader = shader; };

	void LoadDefaultObjects();
	void AddSceneObject(SceneObject* objectToAdd);
	std::vector<SceneObject*> GetSceneObjects() { return this->sceneObjects; };

	void SetMainCamera(Camera* camera) { this->mainCamera = camera; };
	std::vector<Camera*> GetSceneCameras() { return this->sceneCameras; };
	Camera* GetMainCamera() { return this->mainCamera; };
	void AddSceneCamera();

	void SetAspectRatio(float newAspectRatio) { this->aspectRatio = newAspectRatio; };
	float GetAspectRatio() { return this->aspectRatio; };

	bool HasUnsavedChanges() { return !this->upToDate; }

	void ProcessMessage(Message msg);

	void UpdateMousePosition();
	void UpdateScreenProperties();
	void UpdateScene();
	void RenderScene();


	void CloseScene();

private:

	//keeps track of if everything within
	//the scene is updated
	bool upToDate = true;

	//a vector to keep track of SceneObjects
	std::vector<SceneObject*> sceneObjects;

	//a vector to keep track of SceneCameras
	std::vector<Camera*> sceneCameras;
	Camera* mainCamera = nullptr;
	//a value we'll increment every time we add
	//a camera to generate a "unique ID"
	unsigned int cameraIterator = 0;

	//keep track of the scene's shader so we can
	//update variables within the shader during
	//rendering
	Shader* sceneShader = nullptr;

	//keeps track of the aspect ratio, gets updated
	//every time the window changes
	float aspectRatio = 1.0f;

	glm::vec3 GetArcBallVec(double* mousePos);
	double GetArcBallAngle(glm::vec3 arcBallVec);

};

