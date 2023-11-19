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
	Plane* tempPlane = nullptr;
	//load default planes:
	//	X-Y Plane
	tempPlane = new Plane({ 0.0f, 0.0f, 1.0f });
	tempPlane->displayName = "X - Y Plane";
	tempPlane->isDefaultObject = true;
	tempPlane->SetDebugColor({ 255, 0, 0});
	tempPlane->SetFlatColor({ 255, 192, 192 });
	tempPlane->SetObjectScale(10.0f);
	this->AddSceneObject(tempPlane);
	tempPlane = nullptr;
	//	Y-Z Plane
	tempPlane = new Plane({ 1.0f, 0.0f, 0.0f });
	tempPlane->displayName = "Y - Z Plane";
	tempPlane->isDefaultObject = true;
	tempPlane->SetDebugColor({ 0, 255, 0});
	tempPlane->SetFlatColor({ 192, 255, 192 });
	tempPlane->SetObjectScale(10.0f);
	this->AddSceneObject(tempPlane);
	tempPlane = nullptr;
	//	X-Z Plane
	tempPlane = new Plane({ 0.0f, -1.0f, 0.0f });
	tempPlane->displayName = "X - Z Plane";
	tempPlane->isDefaultObject = true;
	tempPlane->SetDebugColor({ 0, 0, 255});
	tempPlane->SetFlatColor({ 192, 192, 255 });
	tempPlane->SetObjectScale(10.0f);
	this->AddSceneObject(tempPlane);
	tempPlane = nullptr;

	//add the default camera to the scene
	this->AddSceneCamera();
}

void CAD_SCENE::AddSceneObject(SceneObject* objectToAdd)
{
	this->sceneObjects.push_back(objectToAdd);
}

void CAD_SCENE::AddSceneCamera()
{
	//add a camera to the scene
	this->sceneCameras.push_back(new Camera(cameraIterator));

	if (cameraIterator == 0)
	{
		this->SetMainCamera(this->sceneCameras[cameraIterator]);
	}
	//increment the counter to ensure unique IDs
	this->cameraIterator++;
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
		viewArcBallOn = (msg.messageData == "PRESSED");

		arcClickLastPos[0] = mousePos[0];
		arcClickLastPos[1] = mousePos[1];
	}

	//mouse when scroll zooms in and out
	if (msg.messageType == "M_SCROLL")
	{
		std::cout << std::log(2 + std::stof(msg.messageData)) << std::endl;

		float zoomDelta = std::stof(msg.messageData);

		if (this->GetMainCamera())
		{
			this->GetMainCamera()->ChangeZoom(zoomDelta);
		}
	}

	
}

void CAD_SCENE::UpdateMousePosition()
{
	glfwGetCursorPos(glfwGetCurrentContext(), &mousePos[0], &mousePos[1]);
	mousePos[0] = std::min(mousePos[0], (double)screenWidth);
	mousePos[1] = std::min(mousePos[1], (double)screenHeight);

	if (viewArcBallOn)
	{

		arcDragVector[0] = arcClickLastPos[0] - mousePos[0];
		arcDragVector[1] = arcClickLastPos[1] - mousePos[1];
	}
}

void CAD_SCENE::UpdateScreenProperties()
{
	glfwGetWindowSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);
	this->SetAspectRatio((float)screenWidth / (float)screenHeight);
}

glm::vec3 CAD_SCENE::GetArcBallVec(double* mousePos)
{
	glm::vec3 ballPoint = glm::vec3(
			(mousePos[0] / ((2.0f) * (double)screenWidth)) - 1.0f,
			-1.0f*((mousePos[1] / ((2.0f) * (double)screenHeight)) - 1.0f),
			0.0f);
	double distToBallCenter = ballPoint.x * ballPoint.x + ballPoint.y * ballPoint.y;

	if (distToBallCenter <= 1.0f)
	{
		ballPoint.z = sqrt(1.0f - distToBallCenter);
	}
	else
	{
		ballPoint = glm::normalize(ballPoint);
	}

	return ballPoint;
}

double CAD_SCENE::GetArcBallAngle(glm::vec3 arcBallVec)
{
	glm::vec3 originalVec = GetArcBallVec(arcClickLastPos);
	return acos(std::min(1.0f, glm::dot(originalVec, arcBallVec)));
}

void CAD_SCENE::UpdateScene()
{
	Camera* cam = this->GetMainCamera();

	this->UpdateScreenProperties();
	this->UpdateMousePosition();
	
	if (cam)
	{
		cam->UpdateCamera();

		//if we have arc ball control for view enabled:
		if (viewArcBallOn)
		{
			double dAngleX = (2.0f * glm::pi<double>()) / (double)screenWidth;
			double dAngleY = (2.0f * glm::pi<double>()) / (double)screenHeight;

			double xAngle = arcDragVector[0] * dAngleX;
			double yAngle = arcDragVector[1] * dAngleY;

			cam->ArcBall(dAngleX, dAngleY, xAngle, yAngle);

			this->arcClickLastPos[0] = mousePos[0];
			this->arcClickLastPos[1] = mousePos[1];
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
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	//clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//get the active camera, and get the "projection
	//matrix" and "view matrix" from that camera:
	//(but only do this if a camera exists!)
	if (!this->sceneCameras.empty())
	{
		//first the projection matrix
		glm::mat4 projectionMatrix = this->GetMainCamera()->GetProjectionMatrix(this->GetAspectRatio());
		//and get the view matrix
		glm::mat4 viewMatrix = this->GetMainCamera()->GetViewMatrix();

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
	

	
}


