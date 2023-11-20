#include "CAD_SCENE.h"

CAD_SCENE::CAD_SCENE()
{
	this->LoadDefaultObjects();
}

CAD_SCENE::CAD_SCENE(std::string loadPath)
{
	this->LoadDefaultObjects();
}

CAD_SCENE::~CAD_SCENE()
{
	//go through each sceneObject
	//and delete them
	for (int idx = this->sceneObjects.size() - 1; idx >= 0; idx--)
	{
		delete sceneObjects[idx];
	}
}

void CAD_SCENE::LoadDefaultObjects()
{
	//SET UP PLANES
	this->LoadDefaultPlanes();

	//SET UP AXES
	this->LoadDefaultAxes();
}

void CAD_SCENE::LoadDefaultPlanes()
{
	Plane* tempPlane = nullptr;
	//load default planes:
	//	X-Y Plane
	tempPlane = new Plane("Z");
	this->AddSceneObject(tempPlane);
	tempPlane = nullptr;
	//	X-Z Plane
	tempPlane = new Plane("Y");
	this->AddSceneObject(tempPlane);
	tempPlane = nullptr;
	//	Y-Z Plane
	tempPlane = new Plane("X");
	this->AddSceneObject(tempPlane);
	tempPlane = nullptr;
}

void CAD_SCENE::LoadDefaultAxes()
{
	Axis* tempAxis = nullptr;
	//load default axes:
	// X Axis
	tempAxis = new Axis("X");
	this->AddSceneObject(tempAxis);
	tempAxis = nullptr;
	// Y Axis
	tempAxis = new Axis("Y");
	this->AddSceneObject(tempAxis);
	tempAxis = nullptr;
	// Z Axis
	tempAxis = new Axis("Z");
	this->AddSceneObject(tempAxis);
	tempAxis = nullptr;
}

void CAD_SCENE::AddSceneObject(SceneObject* objectToAdd)
{
	this->sceneObjects.push_back(objectToAdd);
}

void CAD_SCENE::SetCameraView(Camera::DefinedView desiredView)
{
	if (desiredView == Camera::DefinedView::SAVED)
	{
		this->GetCamera()->GoToDefinedView(desiredView, this->sceneState.SavedCamera);
	}
	else
	{
		this->GetCamera()->GoToDefinedView(desiredView);
	}
}

void CAD_SCENE::CloseScene()
{
}

void CAD_SCENE::ProcessMessage(Message msg)
{
	std::cout << msg.messageType << std::endl;
	

	//Middle mouse toggles the arc ball for manipulating the view
	if (msg.messageType == "M_MOUSE")
	{
		this->sceneState.ArcBallMode = (msg.messageData == "PRESSED");

		this->sceneState.LastArcClick[0] = this->sceneState.MousePos[0];
		this->sceneState.LastArcClick[1] = this->sceneState.MousePos[1];
	}

	//mouse when scroll zooms in and out
	if (msg.messageType == "M_SCROLL")
	{
		std::cout << std::log(2 + std::stof(msg.messageData)) << std::endl;

		float zoomDelta = std::stof(msg.messageData);

		if (this->GetCamera())
		{
			this->GetCamera()->ChangeZoom(zoomDelta);
		}
	}

	
}

void CAD_SCENE::UpdateMousePosition()
{
	glfwGetCursorPos(glfwGetCurrentContext(), &this->sceneState.MousePos[0], &this->sceneState.MousePos[1]);
	this->sceneState.MousePos[0] = std::min(this->sceneState.MousePos[0], (double)this->sceneState.ScreenDimensions[0]);
	this->sceneState.MousePos[0] = std::max(this->sceneState.MousePos[0], (double)0.0f);
	this->sceneState.MousePos[1] = std::min(this->sceneState.MousePos[1], (double)this->sceneState.ScreenDimensions[1]);
	this->sceneState.MousePos[1] = std::max(this->sceneState.MousePos[1], (double)0.0f);

	if (this->sceneState.ArcBallMode)
	{
		this->sceneState.ArcDragVector[0] = this->sceneState.LastArcClick[0] - this->sceneState.MousePos[0];
		this->sceneState.ArcDragVector[1] = this->sceneState.LastArcClick[1] - this->sceneState.MousePos[1];

		this->sceneState.LastArcClick[0] = this->sceneState.MousePos[0];
		this->sceneState.LastArcClick[1] = this->sceneState.MousePos[1];
	}
}

void CAD_SCENE::UpdateScreenProperties()
{
	glfwGetWindowSize(glfwGetCurrentContext(), &this->sceneState.ScreenDimensions[0], &this->sceneState.ScreenDimensions[1]);
	this->sceneState.ScreenAspectRatio = ((float)this->sceneState.ScreenDimensions[0] / (float)this->sceneState.ScreenDimensions[1]);
}

void CAD_SCENE::UpdateScene()
{
	Camera* cam = this->GetCamera();

	this->UpdateScreenProperties();
	this->UpdateMousePosition();
	
	if (cam)
	{
		cam->UpdateCamera();

		//if we have arc ball control for view enabled:
		if (this->sceneState.ArcBallMode)
		{
			double dAngleX = (2.0f * glm::pi<double>()) / (double)this->sceneState.ScreenDimensions[0];
			double dAngleY = (2.0f * glm::pi<double>()) / (double)this->sceneState.ScreenDimensions[1];

			double xAngle = -this->sceneState.ArcDragVector[0] * dAngleX;
			double yAngle = -this->sceneState.ArcDragVector[1] * dAngleY;

			cam->ArcBall(xAngle, yAngle);
		}
	}
	

}

void CAD_SCENE::RenderScene()
{
	//enable openGL depth test
	glEnable(GL_DEPTH_TEST);

	//enable opacity/alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//set the clear color
	glClearColor(0.15f, 0.15f, 0.25f, 1.0f);
	//clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//get the active camera, and get the "projection
	//matrix" and "view matrix" from that camera:
	
	//first the projection matrix
	glm::mat4 projectionMatrix = this->GetCamera()->GetProjectionMatrix(this->GetAspectRatio());
	//and get the view matrix
	glm::mat4 viewMatrix = this->GetCamera()->GetViewMatrix();

	//update the uniforms for the shader with the camera matrices
	this->GetShader()->setMat4("projection", projectionMatrix);
	this->GetShader()->setMat4("view", viewMatrix);

	std::vector<SceneObject*> objectsWithTransparency;

	//now, loop through the SceneObjects
	//and render them
	for (SceneObject* sO : this->GetSceneObjects())
	{
		//only do this if the object is visible:
		if (!sO->isVisible)
		{
			continue;
		}
		//if the object has transparency, add that to the list so we can do them second:
		if (sO->GetIsTransparent())
		{
			objectsWithTransparency.push_back(sO);
			continue;
		}
		//update the -per model- uniforms (i.e.
		//model matrices, colors, etc.)
		//first get the model matrix:
		glm::mat4 modelMatrix = sO->GetModelMatrix();
		//then update the shader
		this->GetShader()->setMat4("model", modelMatrix);

		//get the R G B and A components of the debug color
		//of the model
		float r = (float)sO->GetDebugColor().r;
		float g = (float)sO->GetDebugColor().g;
		float b = (float)sO->GetDebugColor().b;
		float a = (float)sO->GetDebugColor().a;
		//scale them to be between 0 and 1 (divide by 255)
		r /= 255.0f;
		g /= 255.0f;
		b /= 255.0f;
		a /= 255.0f;
		//now, make  the color a vec4
		glm::vec4 debugColor = glm::vec4(r, g, b, a);
		//next, do the debug color to test things
		this->GetShader()->setVec4("debugColor", debugColor);

		//do the same for the flat color (wow, this is a pain! maybe I should fix that.)
		r = (float)sO->GetFlatColor().r;
		g = (float)sO->GetFlatColor().g;
		b = (float)sO->GetFlatColor().b;
		a = (float)sO->GetFlatColor().a;
		//scale them to be between 0 and 1 (divide by 255)
		r /= 255.0f;
		g /= 255.0f;
		b /= 255.0f;
		a /= 255.0f;
		//now, make  the color a vec4
		glm::vec4 flatColor = glm::vec4(r, g, b, a);
		//update the flat color uniform
		this->GetShader()->setVec4("flatColor", flatColor);

		//next, update the shader based on the object type:
		this->GetShader()->setInt("shaderType", sO->GetShaderType());
		//then render the object
		sO->RenderObject();
	}

	//start of transparencies; turn off depth mask
	glDepthMask(false);

	//now do that whole loop for objects with transparency: hint-- should turn this into a
	//function or something!
	for (SceneObject* sO : objectsWithTransparency)
	{
		//only do this if the object is visible:
		if (!sO->isVisible)
		{
			continue;
		}

		//update the -per model- uniforms (i.e.
		//model matrices, colors, etc.)
		//first get the model matrix:
		glm::mat4 modelMatrix = sO->GetModelMatrix();
		//then update the shader
		this->GetShader()->setMat4("model", modelMatrix);

		//get the R G B and A components of the debug color
		//of the model
		float r = (float)sO->GetDebugColor().r;
		float g = (float)sO->GetDebugColor().g;
		float b = (float)sO->GetDebugColor().b;
		float a = (float)sO->GetDebugColor().a;
		//scale them to be between 0 and 1 (divide by 255)
		r /= 255.0f;
		g /= 255.0f;
		b /= 255.0f;
		a /= 255.0f;
		//now, make  the color a vec4
		glm::vec4 debugColor = glm::vec4(r, g, b, a);
		//next, do the debug color to test things
		this->GetShader()->setVec4("debugColor", debugColor);

		//do the same for the flat color (wow, this is a pain! maybe I should fix that.)
		r = (float)sO->GetFlatColor().r;
		g = (float)sO->GetFlatColor().g;
		b = (float)sO->GetFlatColor().b;
		a = (float)sO->GetFlatColor().a;
		//scale them to be between 0 and 1 (divide by 255)
		r /= 255.0f;
		g /= 255.0f;
		b /= 255.0f;
		a /= 255.0f;
		//now, make  the color a vec4
		glm::vec4 flatColor = glm::vec4(r, g, b, a);
		//update the flat color uniform
		this->GetShader()->setVec4("flatColor", flatColor);

		//next, update the shader based on the object type:
		this->GetShader()->setInt("shaderType", sO->GetShaderType());
		//then render the object
		sO->RenderObject();
	}

	//done with transparencies; turn on depth mask again
	glDepthMask(true);
	

	//std::cout << glGetError() << std::endl;
	

	
}


