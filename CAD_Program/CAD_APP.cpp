#include "CAD_APP.h"

MessageManager CAD_APP::messageManager = MessageManager();

void CAD_APP::RegisterCallbacks()
{
	//set the framebuffer size callback
	glfwSetFramebufferSizeCallback(this->GetMainWindow(), windowResizeCallback);

	//set the mouse buttons callback
	glfwSetMouseButtonCallback(this->GetMainWindow(), mouseButtonCallBack);

	//set the mouse scroll wheel callback
	glfwSetScrollCallback(this->GetMainWindow(), CAD_APP::mouseScrollCallback);

}

CAD_APP::~CAD_APP()
{
	this->ShutdownApp();
}

void CAD_APP::MainLoop()
{
	//start a new Dear ImGui frame:
	this->NewImGuiFrame();

	//process input:
	this->ProcessMessages();

	//update application:
	this->Update();

	//render application:
	this->Render();
}

bool CAD_APP::InitializeApp()
{
	if (!this->InitializeGlfw()) { return false; };
	if (!this->InitializeGlad()) { return false; };
	if (!this->InitializeDearImGui()) { return false; };
	
	if (!this->InitializeShader()) { return false; };
	this->defaultApplicationShader->Use();

	//load an empty scene to start
	this->currentScene = this->LoadEmptyScene();

	return true;
}

void CAD_APP::ShutdownApp()
{
	//delete the shader program
	this->defaultApplicationShader->deleteProgram();

	//clean up Dear ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//clean up GLFW
	glfwTerminate();
}

//define callbacks:
void CAD_APP::windowResizeCallback(GLFWwindow* resizedWindow, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	messageManager.ReceiveMessage({ "WINDOW", "RESIZE" });
}

void CAD_APP::mouseButtonCallBack(GLFWwindow* relevantWindow, int button, int action, int mods)
{
	Message msg;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//std::cout << "Left mouse button pressed." << std::endl;
		msg = { "L_MOUSE", "PRESSED" };

	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		//std::cout << "Middle mouse button pressed." << std::endl;
		msg = { "M_MOUSE", "PRESSED" };
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//std::cout << "Right mouse button pressed." << std::endl;
		msg = { "R_MOUSE", "PRESSED" };
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		//std::cout << "Left mouse button released." << std::endl;
		msg = { "L_MOUSE", "RELEASED" };
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		//std::cout << "Middle mouse button released." << std::endl;
		msg = { "M_MOUSE", "RELEASED" };
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		//std::cout << "Right mouse button released." << std::endl;
		msg = { "R_MOUSE", "RELEASED" };
	}

	messageManager.ReceiveMessage(msg);
}

void CAD_APP::mouseScrollCallback(GLFWwindow* relevantWindow, double xOffset, double yOffset)
{
	//std::cout << "Scroll: " << yOffset << std::endl;
	messageManager.ReceiveMessage({ "M_SCROLL", std::to_string(yOffset)});
}

bool CAD_APP::InitializeGlfw()
{
	//initialize and setup GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//for mac:
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	//create the  window:
	this->applicationWindow = glfwCreateWindow(640, 480, "Default Window", NULL, NULL);
	if (!this->GetMainWindow())
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		return false;
	}

	//we made a window, now make that window the current context
	glfwMakeContextCurrent(this->GetMainWindow());

	//register callbacks
	this->RegisterCallbacks();
	

    return true;
}

bool CAD_APP::InitializeGlad()
{
	//load OpenGL pointers with glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	return true;
}

bool CAD_APP::InitializeDearImGui()
{
	//initialize DearImgui:
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//setup platform/rendererr bindings:
	if (!ImGui_ImplGlfw_InitForOpenGL(this->GetMainWindow(), true))
	{
		return false;
	}
	if (!ImGui_ImplOpenGL3_Init("#version 330"))
	{
		return false;
	}

	return true;
}

bool CAD_APP::InitializeShader()
{
	this->defaultApplicationShader = new Shader("shader.vs", "shader.fs");
	if (!this->defaultApplicationShader)
	{
		return false;
	}

	return true;
}

void CAD_APP::InitializeDirectories()
{
	//we'll have a "Data" folder (local relative to the
	//application) where we store things like default
	//window size, preferences, etc.
	std::filesystem::create_directory("/data");
}

void CAD_APP::NewImGuiFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void CAD_APP::ProcessMessages()
{
	//send the latest messages from the manager
	while (CAD_APP::messageManager.QueueHasMessages())
	{
		this->currentScene->ProcessMessage(messageManager.DispatchMessage());
	}
	
}

void CAD_APP::Update()
{
	this->currentScene->UpdateScene();
}

void CAD_APP::Render()
{
	//render application
	this->currentScene->RenderScene();
	
	//render GUI
	this->RenderGUI();

	//swap glfwbuffers
	glfwSwapBuffers(this->GetMainWindow());

	//poll events
	glfwPollEvents();
}

void CAD_APP::RenderOpenGL()
{

}

void CAD_APP::RenderGUI()
{
	//render Dear ImGUI GUI
	//begin a test window -- will eventually become
	//the "scene tree"
	ImGui::Begin("Scene Tree:");

	//first, display the current camera
	std::string camText = "Camera ";
	camText.append(std::to_string(this->GetCurrentScene()->GetMainCamera()->GetID()));
	ImGui::Text(camText.c_str());
	ImGui::SameLine();
	if (this->GetCurrentScene()->GetMainCamera()->GetOrthographicMode() == true)
	{
		if (ImGui::Button("Toggle Perspective"))
		{
			this->GetCurrentScene()->GetMainCamera()->SetPerspectiveMode();
		}
	}
	else if (this->GetCurrentScene()->GetMainCamera()->GetOrthographicMode() == false)
	{
		if (ImGui::Button("Toggle Orthographic"))
		{
			this->GetCurrentScene()->GetMainCamera()->SetOrthographicMode();
		}
	}
	
	float camPos[3] = { this->GetCurrentScene()->GetMainCamera()->GetPosition().x, this->GetCurrentScene()->GetMainCamera()->GetPosition().y, this->GetCurrentScene()->GetMainCamera()->GetPosition().z};
	float fov = this->GetCurrentScene()->GetMainCamera()->GetFOV();
	ImGui::InputFloat3("(X, Y, Z)", camPos);
	ImGui::InputFloat("FOV:", &fov, 2.5f, 45.0f);
	bool test;
	if(ImGui::RadioButton("Test", &test))
	{
		//uhhhh
	}
	float testing;
	if (ImGui::DragFloat("Test 2", &testing))
	{
		//uhhhhhhhhhhhhhh
		glm::vec3 up = this->GetCurrentScene()->GetMainCamera()->GetCameraUp();
	};
	this->GetCurrentScene()->GetMainCamera()->SetPosition({ camPos[0], camPos[1], camPos[2]});
	this->GetCurrentScene()->GetMainCamera()->SetFOV(fov);


	//then list the objects
	for (SceneObject* sO : this->GetCurrentScene()->GetSceneObjects() )
	{
		//if this is a default object, display in a different color:
		if (sO->isDefaultObject)
		{
			ImGui::TextColored({ 0, 200, 200, 200 }, sO->displayName.c_str());
		}
		
		else {
			ImGui::Text(sO->displayName.c_str());
		}
		ImGui::SameLine();
		//now, display what type of object it is (implement this later)
		ImGui::TextColored({ 128, 0, 128, 255 }, ("["+sO->objectType+"]").c_str());
		ImGui::SameLine();
		ImGui::PushID(*sO->GetObjectVBOPointer());
		//check if the object is visible so we know if we should say "Hide" or "Show"
		bool isVisible = sO->isVisible;
		std::string hideShowText;
		if (isVisible) hideShowText = "Hide";
		else hideShowText = "Show";
		if (ImGui::Button(hideShowText.c_str()))
		{
			sO->isVisible = !(sO->isVisible);
		}
			
		
		ImGui::PopID();

		
		
	}

	ImGui::End();

	//actually render DearImGUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool CAD_APP::LoadSceneFromFile(std::string scenePath)
{
	//check if currently a scene open:
	if (this->currentScene)
	{
		//check if current scene needs to be saved:
		if (this->currentScene->HasUnsavedChanges())
		{
			//implement this later
		}

		delete this->currentScene;
		this->currentScene = nullptr;
	}

	return false;
}

CAD_SCENE* CAD_APP::LoadEmptyScene()
{
	CAD_SCENE* emptyScene = new CAD_SCENE();
	emptyScene->SetShader(this->defaultApplicationShader);

	return emptyScene;
}
