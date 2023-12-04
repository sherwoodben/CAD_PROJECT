#include "CAD_SCENE.h"
#include "AppGUI.h"
#include "CAD_APP.h"

#define SCREEN_CLEAR_R 0.5f
#define SCREEN_CLEAR_G 0.5f
#define SCREEN_CLEAR_B 0.8f

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
	//ADD ORIGIN
	Point* origin = new Point({ 0.0f, 0.0f, 0.0f });
	origin->displayName = "Origin";
	origin->isDefaultObject = true;
	origin->SetDebugColor({ 255, 255, 255 });
	origin->SetFlatColor({ 255, 255, 255 });
	this->AddSceneObject(origin);
	origin = nullptr;

	//SET UP AXES
	this->LoadDefaultAxes();

	//SET UP PLANES
	this->LoadDefaultPlanes();

	//TEST SURFACES
	Point3D testPoints1[3] = {
		Point3D(5.0f, 0.0f, 0.0f),
		Point3D(5.0f, 0.0f, 5.0f),
		Point3D(0.0f, 0.0f, 5.0f)
	};

	Point3D testPoints2[3] = {
		Point3D(5.0f, 10.0f, 0.0f),
		Point3D(5.0f, 10.0f, 5.0f),
		Point3D(0.0f, 10.0f, 5.0f)
	};

	Bezier3D* testBezier1 = new Bezier3D();
	testBezier1->SetControlPoints(3, testPoints1);
	testBezier1->displayName = "Curve 1";
	this->AddSceneObject(testBezier1);

	Bezier3D* testBezier2 = new Bezier3D();
	testBezier2->SetControlPoints(3, testPoints2);
	testBezier2->displayName = "Curve 2";
	this->AddSceneObject(testBezier2);

	Ruled* testRuled = new Ruled();
	testRuled->SetCurves(testBezier1, testBezier2);
	testRuled->displayName = "Ruled Surface";
	this->AddSceneObject(testRuled);
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
	if (!objectToAdd->isDatumObject)
	{
		this->sceneObjects.push_back(objectToAdd);
	}
	else
	{
		this->datumObjects.push_back(objectToAdd);
	}
	
}

void CAD_SCENE::DeleteObjects()
{

	for (SceneObject* sO : this->sceneObjects)
	{
		if (sO->canDelete() && sO->tryDelete)
		{
			if (dynamic_cast<Sketch*>(sO) != nullptr)
			{
				Sketch* sketchToRemove = (Sketch*)sO;
				delete sketchToRemove;
			}
			else if (dynamic_cast<Curve3D*>(sO) != nullptr)
			{
				Curve3D* curveToRemove = (Curve3D*)sO;
				delete curveToRemove;
			}
			else if (dynamic_cast<Surface*>(sO) != nullptr)
			{
				Surface* surfaceToRemove = (Surface*)sO;
				delete surfaceToRemove;
			}
		}
		else
		{
			sO->tryDelete = false;
		}
	}

	for (SceneObject* sO : this->datumObjects)
	{
		if (sO->canDelete() && sO->tryDelete)
		{
			if (dynamic_cast<Point*>(sO) != nullptr)
			{
				Point* pointToRemove = (Point*)sO;
				delete pointToRemove;
			}
			else if (dynamic_cast<Axis*>(sO) != nullptr)
			{
				Axis* axisToRemove = (Axis*)sO;
				delete axisToRemove;
			}
			else if (dynamic_cast<Plane*>(sO) != nullptr)
			{
				Plane* planeToRemove = (Plane*)sO;
				delete planeToRemove;
			}
		}
		else
		{
			sO->tryDelete = false;
		}
	}

	std::erase_if(this->sceneObjects, [](SceneObject* x) {return (x->canDelete() && x->tryDelete); });
	

	std::erase_if(this->datumObjects, [](SceneObject* x) {return (x->canDelete() && x->tryDelete); });

}

void CAD_SCENE::DoArcBallCam()
{
	double dAngleX = (2.0f * glm::pi<double>()) / (double)this->sceneState.ScreenDimensions[0];
	double dAngleY = (2.0f * glm::pi<double>()) / (double)this->sceneState.ScreenDimensions[1];

	double xAngle = -this->sceneState.ArcDragVector[0] * dAngleX;
	double yAngle = -this->sceneState.ArcDragVector[1] * dAngleY;

	this->sceneState.SceneCamera.ArcBall(xAngle, yAngle);
}

void CAD_SCENE::DoTranslateCam()
{
	double dDeltaX = 1.0f / (double)this->sceneState.ScreenDimensions[0];
	double dDeltaY = 1.0f / (double)this->sceneState.ScreenDimensions[1];

	double deltaX =  this->sceneState.ArcDragVector[0] * dDeltaX;
	double deltaY = -this->sceneState.ArcDragVector[1] * dDeltaY;

	this->sceneState.SceneCamera.TranslateCam(deltaX, deltaY);
}

void CAD_SCENE::SetCameraView(CameraState::DefinedView desiredView)
{
	if (desiredView == CameraState::DefinedView::SAVED)
	{
		this->sceneState.SceneCamera.GoToDefinedView(desiredView, this->sceneState.SavedView);
	}
	else
	{
		this->sceneState.SceneCamera.GoToDefinedView(desiredView);
	}
}

void CAD_SCENE::CloseScene()
{
}

void CAD_SCENE::ProcessMessage(Message msg)
{
	//std::cout << msg.messageType << " " << msg.messageData << std::endl;
	
	if (msg.messageType == "DELETE")
	{
		this->HandleDeleteObjectMessage(msg.messageData);
		return;
	}
	if (msg.messageType == "NEW_OBJECT")
	{
		this->HandleNewObjectMessage(msg.messageData);
		return;
	}

	//Middle mouse toggles the arc ball for manipulating the view
	if (msg.messageType == "M_MOUSE" || (this->sceneState.AltModifier && msg.messageType == "R_MOUSE"))
	{
		this->sceneState.ArcBallMode = (msg.messageData == "PRESSED");

		this->sceneState.LastArcClick[0] = this->sceneState.MousePos[0];
		this->sceneState.LastArcClick[1] = this->sceneState.MousePos[1];
	}

	//mouse when scroll zooms in and out
	if (msg.messageType == "M_SCROLL")
	{
		float zoomDelta = std::stof(msg.messageData);

		if (&this->sceneState.SceneCamera != nullptr)
		{
			this->sceneState.SceneCamera.ChangeZoom(zoomDelta);
		}
	}

	if (msg.messageType == "SHIFT")
	{
		this->sceneState.ShiftModifier = (msg.messageData == "PRESSED");
	}

	if (msg.messageType == "ALT")
	{
		this->sceneState.AltModifier = (msg.messageData == "PRESSED");
	}

	/*if (msg.messageType == "NUM_1" && msg.messageData == "PRESSED")
	{
		if (!this->sceneState.ShiftModifier)
		{
			this->SetCameraView(CameraState::DefinedView::FRONT);
		}
		else
		{
			this->SetCameraView(CameraState::DefinedView::BACK);
		}
	}
	if (msg.messageType == "NUM_2" && msg.messageData == "PRESSED")
	{
		if (!this->sceneState.ShiftModifier)
		{
			this->SetCameraView(CameraState::DefinedView::RIGHT);
		}
		else
		{
			this->SetCameraView(CameraState::DefinedView::LEFT);
		}
	}
	if (msg.messageType == "NUM_3" && msg.messageData == "PRESSED")
	{
		if (!this->sceneState.ShiftModifier)
		{
			this->SetCameraView(CameraState::DefinedView::TOP);
		}
		else
		{
			this->SetCameraView(CameraState::DefinedView::BOTTOM);
		}
	}
	if (msg.messageType == "NUM_4" && msg.messageData == "PRESSED")
	{
		this->SetCameraView(CameraState::DefinedView::ISOMETRIC);
	}*/
	
}

void CAD_SCENE::HandleDeleteObjectMessage(std::string objToDelete)
{
	for (SceneObject* sO : this->sceneObjects)
	{
		if (sO->displayName == objToDelete)
		{
			sO->tryDelete = true;
			return;
		}
	}

	for (SceneObject* sO : this->datumObjects)
	{
		if (sO->displayName == objToDelete)
		{
			sO->tryDelete = true;
			return;
		}
	}
}

void CAD_SCENE::HandleNewObjectMessage(std::string typeToAdd)
{
	if (typeToAdd == "POINT")
	{
		//std::cout << "Adding Point." << std::endl;
		return;
	}
	if (typeToAdd == "AXIS")
	{
		//std::cout << "Add AXIS Implementation Missing." << std::endl;
		return;
	}
	if (typeToAdd == "PLANE")
	{
		//std::cout << "Add PLANE Implementation Missing." << std::endl;
		return;
	}
	if (typeToAdd == "SKETCH")
	{
		//std::cout << "Add SKETCH Implementation Missing." << std::endl;
		return;
	}
	if (typeToAdd == "SURFACE")
	{
		//std::cout << "Add SURFACE Implementation Missing." << std::endl;
		return;
	}
	if (typeToAdd == "SOLID")
	{
		//std::cout << "Add SOLID Implementation Missing." << std::endl;
		return;
	}

	//std::cout << "UNKNOWN TYPE REQUESTED FOR NEW OBJECT" << std::endl;
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
	this->sceneState.ScreenAspectRatio = ((float)(this->sceneState.ScreenDimensions[0] - 256) / (float)this->sceneState.ScreenDimensions[1]);
}

void CAD_SCENE::UpdateDependents()
{
	for (SceneObject* sO : this->sceneObjects )
	{
		if (!sO->canDelete())
		{
			sO->Update();
		}
	}

	for (SceneObject* sO : this->datumObjects)
	{
		if (!sO->canDelete())
		{
			sO->Update();
		}
	}
}

void CAD_SCENE::UpdateScene()
{
	Camera* cam = &this->sceneState.SceneCamera;

	this->UpdateScreenProperties();
	this->UpdateMousePosition();
	
	if (cam)
	{
		cam->UpdateCamera();

		//if we have arc ball control for view enabled (NO SHIFT):
		//("orbit camera" type mode)
		if (this->sceneState.ArcBallMode && !this->sceneState.ShiftModifier)
		{
			this->DoArcBallCam();
			
		}
		//if we have translate control for view enabled (YES SHIFT):
		//("pan" type mode)
		else if (this->sceneState.ArcBallMode && this->sceneState.ShiftModifier)
		{
			this->DoTranslateCam();
		}
	}

	if (!this->UpToDate)
	{
		for (SceneObject* sO : this->sceneObjects)
		{
			sO->Update();
		}
		for (SceneObject* sO : this->datumObjects)
		{
			sO->Update();
		}
	}
}

void CAD_SCENE::RenderScene()
{
	glViewport(256, 0, sceneState.ScreenDimensions[0] - 256, sceneState.ScreenDimensions[1]);
	//enable openGL depth test
	glEnable(GL_DEPTH_TEST);

	//enable opacity/alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//set the clear color
	glClearColor(SCREEN_CLEAR_R, SCREEN_CLEAR_G, SCREEN_CLEAR_B, 1.0f);
	//clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//get the active camera, and get the "projection
	//matrix" and "view matrix" from that camera:
	
	//first the projection matrix
	glm::mat4 projectionMatrix = this->sceneState.SceneCamera.GetProjectionMatrix(this->sceneState.ScreenAspectRatio);
	//and get the view matrix
	glm::mat4 viewMatrix = this->sceneState.SceneCamera.GetViewMatrix();

	//update the uniforms for the shaders with the camera matrices
	this->parentApplication->flatShader->Use();
	this->parentApplication->flatShader->setMat4("projection", projectionMatrix);
	this->parentApplication->flatShader->setMat4("view", viewMatrix);

	this->parentApplication->planeGridShader->Use();
	this->parentApplication->planeGridShader->setMat4("projection", projectionMatrix);
	this->parentApplication->planeGridShader->setMat4("view", viewMatrix);

	this->parentApplication->texturedPlaneShader->Use();
	this->parentApplication->texturedPlaneShader->setMat4("projection", projectionMatrix);
	this->parentApplication->texturedPlaneShader->setMat4("view", viewMatrix);

	std::vector<SceneObject*> objectsWithTransparency;

	std::vector<SceneObject*>* objectVectors[2] = {&this->sceneObjects, &this->datumObjects};

	//now, loop through the SceneObjects (and Datum Objects)
	//and render them
	for (int i = 0; i < 1 + this->sceneState.ShowDatumObjects; i++)
	{
		for (SceneObject* sO : *objectVectors[i])
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

			//pass different shader to render function depending on object type
			if (sO->objectType == "Point" || sO->objectType == "Axis" || sO->objectType == "Curve" || sO->objectType == "Surface")
			{
				sO->RenderObject(this->parentApplication->flatShader);
			}
			else if (sO->objectType == "Plane")
			{
				sO->RenderObject(this->parentApplication->planeGridShader);
			}
			else if (sO->objectType == "Sketch")
			{
				Sketch* sketch = (Sketch*)sO;

				sketch->RenderSketchObjects(this->parentApplication->inSketchShader);
				sketch->RenderObject(this->parentApplication->texturedPlaneShader);
			}

		}
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

		//pass different shader to render function depending on object type
		if (sO->objectType == "Point" || sO->objectType == "Axis" || sO->objectType == "Curve" || sO->objectType == "Surface")
		{
			sO->RenderObject(this->parentApplication->flatShader);
		}
		else if (sO->objectType == "Plane")
		{
			sO->RenderObject(this->parentApplication->planeGridShader);
		}
		else if (sO->objectType == "Sketch")
		{
			Sketch* sketch = (Sketch*)sO;

			sketch->RenderSketchObjects(this->parentApplication->inSketchShader);
			sketch->RenderObject(this->parentApplication->texturedPlaneShader);
		}
	}

	//done with transparencies; turn on depth mask again
	glDepthMask(true);
	

	//std::cout << glGetError() << std::endl;
	

	
}


