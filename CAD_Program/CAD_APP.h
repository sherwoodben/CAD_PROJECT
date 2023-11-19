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
#include "CAD_SCENE.h"
#include "Shader.h"
#include "Message.h"

class CAD_APP
{
public:
	CAD_APP() {};
	~CAD_APP();

	bool InitializeApp();

	void MainLoop();

	GLFWwindow* GetMainWindow() { return this->applicationWindow; };
	CAD_SCENE* GetCurrentScene() { return this->currentScene; };

	bool ShouldClose() { return glfwWindowShouldClose(this->GetMainWindow()); };

	static MessageManager messageManager;

private:

	GLFWwindow* applicationWindow = nullptr;
	CAD_SCENE* currentScene = nullptr;

	Shader* defaultApplicationShader = nullptr;

	

	
	void ShutdownApp();

	static void windowResizeCallback(GLFWwindow* resizedWindow, int newWidth, int newHeight);
	static void mouseButtonCallBack(GLFWwindow* relevantWindow, int button, int action, int mods);
	static void mouseScrollCallback(GLFWwindow* relevantWindow, double xOffset, double yOffset);

	void RegisterCallbacks();
	bool InitializeGlfw();
	bool InitializeGlad();
	bool InitializeDearImGui();
	bool InitializeShader();

	void InitializeDirectories();

	void NewImGuiFrame();
	void ProcessMessages();
	void Update();
	void Render();
	void RenderOpenGL();
	void RenderGUI();

	bool LoadSceneFromFile(std::string scenePath);
	CAD_SCENE* LoadEmptyScene();

};
