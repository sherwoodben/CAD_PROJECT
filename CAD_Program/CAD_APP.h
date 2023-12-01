#pragma once

//include iostream so we can std::cout things
#include <iostream>
//include string so we can use strings
#include <string>
//include filesystem and fstream so we can manage files
#include <filesystem>
#include <fstream>

//include associated OpenGL files
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//include associated Dear ImGui files
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//now we include the files I made:
#include "AppGUI.h"
#include "CAD_SCENE.h"
#include "Shader.h"
#include "Message.h"

class CAD_APP
{
public:
	//constructor
	CAD_APP() {};
	//and destructor
	~CAD_APP();

	//we need a way to send/receive messages,
	//so we use our MessageManager
	static MessageManager messageManager;

	//"head" initialization function
	bool InitializeApp();

	//"head" main loop function
	void MainLoop();

	//returns if we want to close the window or not
	bool ShouldClose() { return glfwWindowShouldClose(this->applicationWindow); };

	//get pointers to the application window and the current scene
	CAD_SCENE* GetCurrentScene() { return this->currentScene; };

	//get a pointer to the window (need this for AppGUI to work)
	GLFWwindow* GetApplicationWindow() { return this->applicationWindow; };

	MenuFlags* GetMenuFlag() { return &this->appMenuFlags; };

	//pointers to the application shaders
	Shader* flatShader = nullptr;
	Shader* planeGridShader = nullptr;
	Shader* texturedPlaneShader = nullptr;
	Shader* inSketchShader = nullptr;

private:
	//pointers to the main window and current scene
	GLFWwindow* applicationWindow = nullptr;
	CAD_SCENE* currentScene = nullptr;


	//static Shader* applicationShaders[];

	//Application GUI flag(s)
	MenuFlags appMenuFlags;

	//initialization functions
	bool InitializeGlfw();
	bool InitializeGlad();
	bool InitializeDearImGui();
	bool InitializeShaders();
	void InitializeDirectories();

	//register callback "head" function
	//(happens in initialization)
	void RegisterCallbacks();
	//callback functions
	static void windowResizeCallback(GLFWwindow* resizedWindow, int newWidth, int newHeight);
	static void mouseButtonCallback(GLFWwindow* relevantWindow, int button, int action, int mods);
	static void mouseScrollCallback(GLFWwindow* relevantWindow, double xOffset, double yOffset);
	static void keyCallback(GLFWwindow* relevantWindow, int key, int scanCode, int action, int mods);

	//start a new DearImgui frame
	void NewImGuiFrame();

	//update the application
	void Update();

	//late update for the application
	void LateUpdate();

	//render the application
	void Render();
	//render the DearImgui portion of
	//the application
	void RenderGUI();

	//load scene in various ways
	bool LoadSceneFromFile(std::string scenePath);
	CAD_SCENE* LoadEmptyScene();

	//shutdown the app
	void ShutdownApp();

};
