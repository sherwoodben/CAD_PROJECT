#include "CAD_APP.h"

#include "Camera.h"

#define MIN_WIDTH 640
#define MIN_HEIGHT 480
#define SCENE_TREE_WIDTH 256

MessageManager CAD_APP::messageManager = MessageManager();

void CAD_APP::RegisterCallbacks()
{
	//set the framebuffer size callback
	glfwSetFramebufferSizeCallback(this->applicationWindow, CAD_APP::windowResizeCallback);

	//set the mouse buttons callback
	glfwSetMouseButtonCallback(this->applicationWindow, CAD_APP::mouseButtonCallback);

	//set the mouse scroll wheel callback
	glfwSetScrollCallback(this->applicationWindow, CAD_APP::mouseScrollCallback);

	//set the keyboard press callback
	glfwSetKeyCallback(this->applicationWindow, CAD_APP::keyCallback);

}

CAD_APP::~CAD_APP()
{
	this->ShutdownApp();
}

void CAD_APP::MainLoop()
{
	//start a new Dear ImGui frame:
	this->NewImGuiFrame();

	//update application:
	this->Update();

	//render application:
	this->Render();

	//do the late update (i.e. deleting objects)
	this->LateUpdate();
}

bool CAD_APP::InitializeApp()
{
	if (!this->InitializeGlfw()) { return false; };
	if (!this->InitializeGlad()) { return false; };
	if (!this->InitializeDearImGui()) { return false; };
	
	if (!this->InitializeShaders()) { return false; };
	
	//default to the flat shader
	this->flatShader->Use();

	//load an empty scene to start
	this->currentScene = this->LoadEmptyScene();

	return true;
}

void CAD_APP::ShutdownApp()
{
	//delete the shader program
	this->flatShader->deleteProgram();
	this->planeGridShader->deleteProgram();
	this->inSketchShader->deleteProgram();
	this->texturedPlaneShader->deleteProgram();

	//clean up Dear ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//clean up GLFW
	glfwTerminate();

	delete this->currentScene;
}

//define callbacks:
void CAD_APP::windowResizeCallback(GLFWwindow* resizedWindow, int newWidth, int newHeight)
{
	if (newWidth < MIN_WIDTH)
	{
		newWidth = MIN_WIDTH;
		
	}
	if (newHeight < MIN_HEIGHT)
	{
		newHeight = MIN_HEIGHT;
	}

	glfwSetWindowSize(resizedWindow, newWidth, newHeight);
	glViewport(0, 0, newWidth, newHeight);
	messageManager.ReceiveMessage({ "WINDOW", "RESIZE" });
}

void CAD_APP::mouseButtonCallback(GLFWwindow* relevantWindow, int button, int action, int mods)
{
	Message msg;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//std::cout << "Left mouse button pressed." << std::endl;
		msg = { "L_MOUSE", "PRESSED" };

	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		//std::cout << "Middle mouse button pressed." << std::endl;
		msg = { "M_MOUSE", "PRESSED" };
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//std::cout << "Right mouse button pressed." << std::endl;
		msg = { "R_MOUSE", "PRESSED" };
	}

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		//std::cout << "Left mouse button released." << std::endl;
		msg = { "L_MOUSE", "RELEASED" };
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		//std::cout << "Middle mouse button released." << std::endl;
		msg = { "M_MOUSE", "RELEASED" };
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
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
	//std::cout << yOffset << std::endl;
}

void CAD_APP::keyCallback(GLFWwindow* relevantWindow, int key, int scanCode, int action, int mods)
{
	Message msg;

	//set the "message type" based on the shift key
	if (key == GLFW_KEY_LEFT_SHIFT)
	{
		msg.messageType = "SHIFT";
		
	}
	//or the alt key
	else if (key == GLFW_KEY_LEFT_ALT)
	{
		msg.messageType = "ALT";
	}
	else if (key == GLFW_KEY_1)
	{
		msg.messageType = "NUM_1";
	}
	else if (key == GLFW_KEY_2)
	{
		msg.messageType = "NUM_2";
	}
	else if (key == GLFW_KEY_3)
	{
		msg.messageType = "NUM_3";
	}
	else if (key == GLFW_KEY_4)
	{
		msg.messageType = "NUM_4";
	}
	else if (key == GLFW_KEY_5)
	{
		msg.messageType = "NUM_5";
	}
	else if (key == GLFW_KEY_6)
	{
		msg.messageType = "NUM_6";
	}
	else if (key == GLFW_KEY_7)
	{
		msg.messageType = "NUM_7";
	}
	else if (key == GLFW_KEY_8)
	{
		msg.messageType = "NUM_8";
	}
	else if (key == GLFW_KEY_9)
	{
		msg.messageType = "NUM_9";
	}

	//ignore the "repeat" action
	if (action == GLFW_PRESS)
	{
		msg.messageData = "PRESSED";
	}
	else if (action == GLFW_RELEASE)
	{
		msg.messageData = "RELEASED";
	}

	if (msg.messageType != "")
	{
		messageManager.ReceiveMessage(msg);
	}



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
	this->applicationWindow = glfwCreateWindow(1280, 720, "Default Window", NULL, NULL);
	if (!this->applicationWindow)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		return false;
	}

	glfwSetWindowTitle(this->applicationWindow, "CAD++");

	//we made a window, now make that window the current context
	glfwMakeContextCurrent(this->applicationWindow);

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
	io.IniFilename = NULL;
	io.LogFilename = NULL;

	//setup platform/rendererr bindings:
	if (!ImGui_ImplGlfw_InitForOpenGL(this->applicationWindow, true))
	{
		return false;
	}
	if (!ImGui_ImplOpenGL3_Init("#version 330"))
	{
		return false;
	}

	return true;
}

bool CAD_APP::InitializeShaders()
{
	this->flatShader = new Shader("shaders\\shader.vs", "shaders\\flat_shader.fs");
	if (!this->flatShader)
	{
		return false;
	}

	this->planeGridShader = new Shader("shaders\\shader.vs", "shaders\\plane_shader.fs");
	if (!this->planeGridShader)
	{
		return false;
	}

	this->inSketchShader = new Shader("shaders\\sketch_from_objects.vs", "shaders\\sketch_from_objects.fs");
	if (!this->inSketchShader)
	{
		return false;
	}

	this->texturedPlaneShader = new Shader("shaders\\shader.vs", "shaders\\plane_textured.fs");
	if (!this->texturedPlaneShader)
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
	//(never actually called for now)
	//std::filesystem::create_directory("/data");
}

void CAD_APP::NewImGuiFrame()
{
	if (glfwGetWindowAttrib(this->applicationWindow, GLFW_ICONIFIED))
	{
		return;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void CAD_APP::Update()
{
	if (glfwGetWindowAttrib(this->applicationWindow, GLFW_ICONIFIED))
	{
		return;
	}
	//send the latest messages from the manager
	while (CAD_APP::messageManager.QueueHasMessages())
	{
		this->currentScene->ProcessMessage(messageManager.DispatchMessage());
	}

	this->currentScene->UpdateScene();
}

void CAD_APP::LateUpdate()
{
	this->currentScene->DeleteObjects();
}

void CAD_APP::Render()
{

	if (glfwGetWindowAttrib(this->applicationWindow, GLFW_ICONIFIED))
	{
		glfwPollEvents();
		return;
	}

	//render application
	this->currentScene->RenderScene();
	
	//render GUI
	this->RenderGUI();

	//swap glfwbuffers
	glfwSwapBuffers(this->applicationWindow);

	//poll events
	glfwPollEvents();
}

void CAD_APP::RenderGUI()
{
	float yOffset;
	AppGUI::RenderMenuBar(yOffset, this);

	AppGUI::RenderSceneTree(SCENE_TREE_WIDTH, yOffset, this);

	if (this->appMenuFlags.newPointDialogue)
	{
		AppGUI::NewPointDialogue(this->currentScene);
	}
	else if (this->appMenuFlags.editPointDialogue)
	{
		Point* pointToEdit = dynamic_cast<Point*>(this->appMenuFlags.selectedObject1);
		AppGUI::EditPointDialogue(this->currentScene, pointToEdit);
	}
	else if (this->appMenuFlags.newAxisDialogue)
	{
		AppGUI::NewAxisDialogue(this->currentScene);
	}
	else if (this->appMenuFlags.editAxisDialogue)
	{
		Axis* axisToEdit = dynamic_cast<Axis*>(this->appMenuFlags.selectedObject1);
		AppGUI::EditAxisDialogue(this->currentScene, axisToEdit);
	}
	else if (this->appMenuFlags.newPlaneDialogue)
	{
		AppGUI::NewPlaneDialogue(this->currentScene);
	}
	else if (this->appMenuFlags.editPlaneDialogue)
	{
		Plane* planeToEdit = dynamic_cast<Plane*>(this->appMenuFlags.selectedObject1);
		AppGUI::EditPlaneDialogue(this->currentScene, planeToEdit);
	}
	else if (this->appMenuFlags.newCurveDialogue)
	{
		AppGUI::NewCurveDialogue(this->currentScene);
	}
	else if (this->appMenuFlags.editCurveDialogue)
	{
		Curve3D* curveToEdit = dynamic_cast<Curve3D*>(this->appMenuFlags.selectedObject1);
		AppGUI::EditCurveDialogue(this->currentScene, curveToEdit);
	}
	else if (this->appMenuFlags.newSketchDialogue)
	{
		AppGUI::NewSketchDialogue(this->currentScene);
	}
	else if (this->appMenuFlags.editSketchDialogue)
	{
		Sketch* sketchToEdit = dynamic_cast<Sketch*>(this->appMenuFlags.selectedObject1);
		AppGUI::EditSketchDialogue(this->currentScene, sketchToEdit);
	}
	else if (this->appMenuFlags.newSketchPointDialogue)
	{
		AppGUI::NewSketchPointDialogue(this->currentScene,  this->currentSketch);
	}
	else if (this->appMenuFlags.editSketchPointDialogue)
	{
		SketchPoint* sketchPointToEdit = dynamic_cast<SketchPoint*>(this->appMenuFlags.selectedSketchObject1);
		AppGUI::EditSketchPointDialogue(this->currentScene, this->currentSketch, sketchPointToEdit);
	}
	else if (this->appMenuFlags.newSketchLineDialogue)
	{
		AppGUI::NewSketchLineDialogue(this->currentScene, this->currentSketch);
	}
	else if (this->appMenuFlags.editSketchLineDialogue)
	{
		SketchLine* sketchLineToEdit = dynamic_cast<SketchLine*>(this->appMenuFlags.selectedSketchObject1);
		AppGUI::EditSketchLineDialogue(this->currentScene, this->currentSketch, sketchLineToEdit);
	}
	else if (this->appMenuFlags.newSketchCurveDialogue)
	{
		AppGUI::NewSketchPointDialogue(this->currentScene, this->currentSketch);
	}
	else if (this->appMenuFlags.editSketchCurveDialogue)
	{
		SketchCurve* sketchCurveToEdit = dynamic_cast<SketchCurve*>(this->appMenuFlags.selectedSketchObject1);
		AppGUI::EditSketchCurveDialogue(this->currentScene, this->currentSketch, sketchCurveToEdit);
	}
	else if (this->appMenuFlags.newSurfaceDialogue)
	{
		AppGUI::NewSurfaceDialogue(this->currentScene);
	}
	else if (this->appMenuFlags.newRuledDialogue)
	{
		AppGUI::NewRuledDialogue(this->currentScene);
	}
	//render Dear ImGUI GUI
	//begin a test window -- will eventually become
	//the "scene tree" --bit of a misnomer for now

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
		if (this->currentScene->HasChanges())
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
	CAD_SCENE* emptyScene = new CAD_SCENE(this);

	return emptyScene;
}
