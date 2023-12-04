#include "AppGUI.h"
#include "CAD_APP.h"
#include "CAD_SCENE.h"
#include "Point.h"
#include "Axis.h"
#include "Plane.h"
#include "3DCurves.h"
#include "Sketch.h"
#include "Surface.h"

#define MAX_NAME_LENGTH 64
#define WARNING_COLOR { 1.0f, 1.0f, 0.0f, 1.0f }
#define INVALID_NAME_WARNING "Name is invalid!"

float MenuFlags::tempPoint1[3] = { 0.0f, 0.0f, 0.0f };
float MenuFlags::tempPoint2[3] = {0.0f, 0.0f, 0.0f};
float MenuFlags::tempPoint3[3] = { 0.0f, 0.0f, 0.0f };

char MenuFlags::defaultObjectName[MAX_NAME_LENGTH] = "";
char MenuFlags::extraCharArray[MAX_NAME_LENGTH] = "";

Point3D MenuFlags::controlPoints[MAX_CTRL_PTS];
SketchPoint MenuFlags::controlPoints2D[MAX_CTRL_PTS];

float MenuFlags::ctrPtsAsArray[MAX_CTRL_PTS][3];

bool MenuFlags::isReversed = false;

int MenuFlags::numControlPoints = 2;
int MenuFlags::polynomialOrder = 1;

SceneObject* MenuFlags::selectedObject1 = nullptr;
SceneObject* MenuFlags::selectedObject2 = nullptr;
SceneObject* MenuFlags::selectedObject3 = nullptr;

SketchObject* MenuFlags::selectedSketchObject1 = nullptr;
SketchObject* MenuFlags::selectedSketchObject2 = nullptr;
SketchObject* MenuFlags::selectedSketchObject3 = nullptr;

void AppGUI::RenderMenuBar(float& yOffset, CAD_APP* currentAppInstance)
{
	if (ImGui::BeginMainMenuBar())
	{
		//file
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				currentAppInstance->RestartApp();
			}
			if (ImGui::MenuItem("Open", 0, nullptr, 0))
			{

			}
			if (ImGui::MenuItem("Save", 0, nullptr, 0))
			{

			}
			if (ImGui::MenuItem("Save As", 0, nullptr, 0))
			{

			}
			if (ImGui::MenuItem("Quit"))
			{
				glfwSetWindowShouldClose(currentAppInstance->GetApplicationWindow(), GLFW_TRUE);
			}
			ImGui::EndMenu();
		}
		//edit
		if (ImGui::BeginMenu("Edit##menuBar"))
		{
			if ( !currentAppInstance->GetMenuFlag()->inSketchMode && ImGui::BeginMenu("New Datum"))
			{
				if (ImGui::MenuItem("Point"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "POINT" });
					currentAppInstance->GetMenuFlag()->newPointDialogue = true;
				}
				if (ImGui::MenuItem("Axis"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "AXIS" });
					currentAppInstance->GetMenuFlag()->newAxisDialogue = true;
				}
				if (ImGui::MenuItem("Plane"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "PLANE" });
					currentAppInstance->GetMenuFlag()->newPlaneDialogue = true;
				}
				ImGui::EndMenu();
			}
			else if (currentAppInstance->GetMenuFlag()->inSketchMode && ImGui::BeginMenu("New "))
			{
				if (ImGui::MenuItem("Point"))
				{
					//currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "POINT" });
					currentAppInstance->GetMenuFlag()->newSketchPointDialogue = true;
				}
				if (ImGui::MenuItem("Line"))
				{
					//currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "AXIS" });
					currentAppInstance->GetMenuFlag()->newSketchLineDialogue = true;
				}
				if (ImGui::MenuItem("Curve"))
				{
					//currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "PLANE" });
					currentAppInstance->GetMenuFlag()->newSketchCurveDialogue = true;
				}
				ImGui::EndMenu();
			}

			if (!currentAppInstance->GetMenuFlag()->inSketchMode && ImGui::BeginMenu("New Feature/Object"))
			{
				if (ImGui::MenuItem("Curve"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "SKETCH" });
					currentAppInstance->GetMenuFlag()->newCurveDialogue = true;
				}
				if (ImGui::MenuItem("Sketch", 0, nullptr, 0))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "SKETCH" });
					currentAppInstance->GetMenuFlag()->newSketchDialogue = true;
				}
				if (ImGui::MenuItem("Surface"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "SURFACE" });
					currentAppInstance->GetMenuFlag()->newSurfaceDialogue = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		//edit
		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Preferences", 0, nullptr, 0))
			{
				currentAppInstance->GetMenuFlag()->preferencesMenu = true;
			}
			ImGui::EndMenu();
		}
		//help
		if (ImGui::BeginMenu("View"))
		{
			if (currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraIsOrthographic &&
				ImGui::MenuItem("Toggle Perspective"))
			{
				currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.SetPerspectiveMode();
			}
			else if (!currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraIsOrthographic &&
				ImGui::MenuItem("Toggle Orthographic"))
			{
				currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.SetOrthographicMode();
			}

			if (ImGui::BeginMenu("Default View(s)"))
			{
				AppGUI::CameraViewSubmenu(currentAppInstance);

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Hide All Datum Objects"))
			{
				for (SceneObject* sO : currentAppInstance->GetCurrentScene()->GetDatumObjects() )
				{
					sO->isVisible = false;
				}
			}
			
			if (ImGui::MenuItem("Save View"))
			{
				currentAppInstance->GetCurrentScene()->SaveCameraView();
			}

			if (ImGui::MenuItem("Reset Camera"))
			{
				currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::RESET);
			}
			ImGui::EndMenu();
		}

		yOffset = ImGui::GetWindowSize().y;
		
		ImGui::EndMainMenuBar();

		if (currentAppInstance->GetMenuFlag()->preferencesMenu)
		{
			AppGUI::PreferencesMenu(currentAppInstance);
		}
		
	}
}

void AppGUI::PreferencesMenu(CAD_APP* currentAppInstance)
{
	ImGui::OpenPopup("Preferences");

	if (ImGui::BeginPopupModal("Preferences", &(currentAppInstance->GetMenuFlag())->preferencesMenu))
	{
		if (ImGui::Button("Close"))
		{
			currentAppInstance->GetMenuFlag()->preferencesMenu = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void AppGUI::CameraViewSubmenu(CAD_APP* currentAppInstance)
{
	if (ImGui::MenuItem("Front"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::FRONT);
	}
	if (ImGui::MenuItem("Right"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::RIGHT);
	}
	if (ImGui::MenuItem("Top"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::TOP);
	}
	if (ImGui::MenuItem("Isometric"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::ISOMETRIC);
	}
	if (ImGui::MenuItem("Back"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::BACK);
	}
	if (ImGui::MenuItem("Left"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::LEFT);
	}
	if (ImGui::MenuItem("Bottom"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::BOTTOM);
	}
	if (ImGui::MenuItem("Saved"))
	{
		currentAppInstance->GetCurrentScene()->SetCameraView(CameraState::DefinedView::SAVED);
	}
}

void AppGUI::RenderSceneTree(int regionWidth, float yOffset, CAD_APP* currentAppInstance)
{
	ImGui::SetNextWindowPos({ 0.0f, (float)currentAppInstance->GetCurrentScene()->sceneState.ScreenDimensions[1]}, NULL, {0.0f, 1.0f});
	ImGui::SetNextWindowSize({ (float)regionWidth, (float)currentAppInstance->GetCurrentScene()->sceneState.ScreenDimensions[1] - yOffset});
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::Begin("SCENE TREE", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
	
	if (currentAppInstance->GetMenuFlag()->inSketchMode)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if (ImGui::TreeNode("Sketch Mode##SKETCHMODE"))
		{
			//list the sketch objects

			for (SketchObject* sO : currentAppInstance->currentSketch->sketchObjects)
			{
				std::string label = sO->GetName().append("##"+std::to_string(sO->objectID));
				if (ImGui::Selectable(label.c_str(), sO->isSelected, 0, ImGui::CalcTextSize(sO->GetName().c_str())))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "SKETCH_SELECT", sO->GetNameAndID() });
				}
				//now, number ID of the object
				ImGui::SameLine();
				ImGui::TextColored({ 128, 0, 128, 255 }, ("[" + std::to_string(sO->objectID) + "]").c_str());
				ImGui::SameLine();
				ImGui::PushID(sO->GetNameAndID().c_str());
				ImGui::SetCursorPosX({ (float)regionWidth - 1.5f * ImGui::CalcTextSize("...")[0] });
				if (ImGui::BeginMenu("..."))
				{
					if (ImGui::MenuItem("Edit##tree"))
					{
						currentAppInstance->GetMenuFlag()->selectedSketchObject1 = sO;
						for (int i = 0; i < MAX_NAME_LENGTH; i++)
						{
							if (i < sO->displayName.length())
							{
								MenuFlags::defaultObjectName[i] = sO->displayName[i];
							}
							else
							{
								MenuFlags::defaultObjectName[i] = '\0';

							}
						}

						if (dynamic_cast<SketchPoint*>(sO) != nullptr)
						{
							MenuFlags::tempPoint1[0] = dynamic_cast<SketchPoint*>(sO)->pos.x;
							MenuFlags::tempPoint1[1] = dynamic_cast<SketchPoint*>(sO)->pos.y;

							MenuFlags::selectedSketchObject1 = sO;

							currentAppInstance->GetMenuFlag()->editSketchPointDialogue = true;
						}
						else if (dynamic_cast<SketchLine*>(sO) != nullptr)
						{
							MenuFlags::tempPoint1[0] = dynamic_cast<SketchLine*>(sO)->p1.pos.x;
							MenuFlags::tempPoint1[1] = dynamic_cast<SketchLine*>(sO)->p1.pos.y;

							MenuFlags::tempPoint2[0] = dynamic_cast<SketchLine*>(sO)->p2.pos.x;
							MenuFlags::tempPoint2[1] = dynamic_cast<SketchLine*>(sO)->p2.pos.y;

							currentAppInstance->GetMenuFlag()->editSketchLineDialogue = true;
						}
						if (dynamic_cast<SketchBSpline*>(sO) != nullptr)
						{
							for (int ctrlPointIdx = 0; ctrlPointIdx < dynamic_cast<SketchBSpline*>(sO)->numControlPoints; ctrlPointIdx++)
							{
								MenuFlags::controlPoints2D[ctrlPointIdx] = dynamic_cast<SketchBSpline*>(sO)->controlPoints[ctrlPointIdx];
							}

							MenuFlags::numControlPoints = dynamic_cast<SketchBSpline*>(sO)->numControlPoints;
							MenuFlags::polynomialOrder = dynamic_cast<SketchBSpline*>(sO)->polynomialOrder;

							currentAppInstance->GetMenuFlag()->editSketchCurveDialogue = true;

						}
						else if (dynamic_cast<SketchBezier*>(sO) != nullptr)
						{
							for (int ctrlPointIdx = 0; ctrlPointIdx < dynamic_cast<SketchBezier*>(sO)->numControlPoints; ctrlPointIdx++)
							{
								MenuFlags::controlPoints2D[ctrlPointIdx] = dynamic_cast<SketchBezier*>(sO)->controlPoints[ctrlPointIdx];
								MenuFlags::ctrPtsAsArray[ctrlPointIdx][0] = dynamic_cast<SketchBezier*>(sO)->controlPoints[ctrlPointIdx].pos.x;
								MenuFlags::ctrPtsAsArray[ctrlPointIdx][1] = dynamic_cast<SketchBezier*>(sO)->controlPoints[ctrlPointIdx].pos.y;

								MenuFlags::numControlPoints = dynamic_cast<SketchBezier*>(sO)->numControlPoints;
								MenuFlags::polynomialOrder = dynamic_cast<SketchBezier*>(sO)->polynomialOrder;

								currentAppInstance->GetMenuFlag()->editSketchCurveDialogue = true;

							}
						}
					}

					if (ImGui::MenuItem("Delete", nullptr, nullptr, !sO->canDelete()))
					{
						currentAppInstance->messageManager.ReceiveMessage({ "DELETE", sO->displayName });
						sO->tryDelete = true;
					}

					ImGui::EndMenu();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();

			if (ImGui::Button("Exit Sketch Mode"))
			{
				AppGUI::ResetDefaultValues();

				currentAppInstance->GetMenuFlag()->inSketchMode = false;
				currentAppInstance->currentSketch = nullptr;
				currentAppInstance->GetMenuFlag()->newSketchPointDialogue = false;
				currentAppInstance->GetMenuFlag()->editSketchPointDialogue = false;
				currentAppInstance->GetMenuFlag()->newSketchLineDialogue = false;
				currentAppInstance->GetMenuFlag()->editSketchLineDialogue = false;
				currentAppInstance->GetMenuFlag()->newSketchCurveDialogue = false;
				currentAppInstance->GetMenuFlag()->editSketchCurveDialogue = false;
			}
		}
	}

	else
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Datum Objects"))
		{
			//list the objects
			for (SceneObject* sO : currentAppInstance->GetCurrentScene()->GetDatumObjects())
			{

				ImGui::PushID(sO->GetObjectVAOPointer());

				if (sO->isDefaultObject)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f });
				}
				if (ImGui::Selectable(sO->displayName.c_str(), sO->isSelected, 0, ImGui::CalcTextSize(sO->displayName.c_str())))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "SELECT", sO->displayName });
				}
				if (sO->isDefaultObject)
				{
					ImGui::PopStyleColor();
				}
				ImGui::PopID();
				//if this is a default object, display in a different color:
				ImGui::SameLine();
				//now, display what type of object it is
				ImGui::TextColored({ 128, 0, 128, 255 }, ("[" + sO->objectType + "]").c_str());
				ImGui::SameLine();
				ImGui::PushID(sO->GetObjectVBOPointer());
				ImGui::SetCursorPosX({ (float)regionWidth - 1.5f * ImGui::CalcTextSize("...")[0] });
				if (ImGui::BeginMenu("..."))
				{
					if (sO->isVisible && ImGui::MenuItem("Hide##tree"))
					{
						sO->isVisible = false;
					}
					else if (!sO->isVisible && ImGui::MenuItem("Show##tree"))
					{
						sO->isVisible = true;
					}
					if (!sO->isDefaultObject && ImGui::MenuItem("Edit##tree"))
					{
						sO->isVisible = true;
						currentAppInstance->GetMenuFlag()->selectedObject1 = sO;
						for (int i = 0; i < MAX_NAME_LENGTH; i++)
						{
							if (i < sO->displayName.length())
							{
								MenuFlags::defaultObjectName[i] = sO->displayName[i];
							}
							else
							{
								MenuFlags::defaultObjectName[i] = '\0';

							}
						}

						if (dynamic_cast<Point*>(sO) != nullptr)
						{
							MenuFlags::tempPoint1[0] = dynamic_cast<Point*>(sO)->GetObjectPosition().x;
							MenuFlags::tempPoint1[1] = dynamic_cast<Point*>(sO)->GetObjectPosition().y;
							MenuFlags::tempPoint1[2] = dynamic_cast<Point*>(sO)->GetObjectPosition().z;

							currentAppInstance->GetMenuFlag()->editPointDialogue = true;
						}
						else if (dynamic_cast<Axis*>(sO) != nullptr)
						{
							MenuFlags::tempPoint1[0] = dynamic_cast<Axis*>(sO)->GetObjectPosition().x;
							MenuFlags::tempPoint1[1] = dynamic_cast<Axis*>(sO)->GetObjectPosition().y;
							MenuFlags::tempPoint1[2] = dynamic_cast<Axis*>(sO)->GetObjectPosition().z;

							MenuFlags::tempPoint2[0] = dynamic_cast<Axis*>(sO)->GetAxisDirection().x;
							MenuFlags::tempPoint2[1] = dynamic_cast<Axis*>(sO)->GetAxisDirection().y;
							MenuFlags::tempPoint2[2] = dynamic_cast<Axis*>(sO)->GetAxisDirection().z;


							currentAppInstance->GetMenuFlag()->editAxisDialogue = true;
						}
						else if (dynamic_cast<Plane*>(sO) != nullptr)
						{
							glm::vec3 center = dynamic_cast<Plane*>(sO)->GetObjectPosition();
							glm::vec3 right = dynamic_cast<Plane*>(sO)->GetPlaneRightVector();
							glm::vec3 up = dynamic_cast<Plane*>(sO)->GetPlaneUpVector();



							MenuFlags::tempPoint1[0] = dynamic_cast<Plane*>(sO)->GetObjectPosition().x;
							MenuFlags::tempPoint1[1] = dynamic_cast<Plane*>(sO)->GetObjectPosition().y;
							MenuFlags::tempPoint1[2] = dynamic_cast<Plane*>(sO)->GetObjectPosition().z;

							MenuFlags::tempPoint2[0] = dynamic_cast<Plane*>(sO)->GetObjectPosition().x + right.x;
							MenuFlags::tempPoint2[1] = dynamic_cast<Plane*>(sO)->GetObjectPosition().y + right.y;
							MenuFlags::tempPoint2[2] = dynamic_cast<Plane*>(sO)->GetObjectPosition().z + right.z;

							MenuFlags::tempPoint3[0] = dynamic_cast<Plane*>(sO)->GetObjectPosition().x + up.x;
							MenuFlags::tempPoint3[1] = dynamic_cast<Plane*>(sO)->GetObjectPosition().y + up.y;
							MenuFlags::tempPoint3[2] = dynamic_cast<Plane*>(sO)->GetObjectPosition().z + up.z;


							currentAppInstance->GetMenuFlag()->editPlaneDialogue = true;
						}
					}
					if (sO->isVisible && ImGui::MenuItem("Focus View"))
					{
						glm::vec3 camOffset = currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition;
						camOffset -= currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetTarget();

						currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.SetTarget(sO->GetObjectPosition());
					}
					if (ImGui::MenuItem("Delete", nullptr, nullptr, (!(sO->isDefaultObject) || !(sO->canDelete()))))
					{
						currentAppInstance->messageManager.ReceiveMessage({ "DELETE", sO->displayName });
					}
					ImGui::EndMenu();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Derived Objects"))
		{
			//list the objects
			for (SceneObject* sO : currentAppInstance->GetCurrentScene()->GetSceneObjects())
			{
				if (sO == nullptr)
				{
					continue;
				}
				ImGui::PushID(sO->GetObjectVAOPointer());

				if (sO->isDefaultObject)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f });
				}
				if (ImGui::Selectable(sO->displayName.c_str(), sO->isSelected, 0, ImGui::CalcTextSize(sO->displayName.c_str())))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "SELECT", sO->displayName });
				}
				if (sO->isDefaultObject)
				{
					ImGui::PopStyleColor();
				}
				ImGui::PopID();
				//if this is a default object, display in a different color:
				ImGui::SameLine();
				//now, display what type of object it is
				ImGui::TextColored({ 128, 0, 128, 255 }, ("[" + sO->objectType + "]").c_str());
				ImGui::SameLine();
				ImGui::PushID(sO->GetObjectVBOPointer());
				ImGui::SetCursorPosX({ (float)regionWidth - 1.5f * ImGui::CalcTextSize("...")[0] });
				if (ImGui::BeginMenu("..."))
				{
					if (sO->objectType == "Sketch" && ImGui::MenuItem("Enter Sketch Mode"))
					{
						currentAppInstance->GetMenuFlag()->inSketchMode = true;
						currentAppInstance->currentSketch = (Sketch*)sO;
						//currentAppInstance->GetMenuFlag()->selectedObject1 = sO;
						sO->isVisible = true;
					}
					if (sO->isVisible && ImGui::MenuItem("Hide"))
					{
						sO->isVisible = false;
					}
					else if (!sO->isVisible && ImGui::MenuItem("Show"))
					{
						sO->isVisible = true;
					}
					if (sO->objectType == "Sketch" && ImGui::MenuItem("Change Sketch Plane"))
					{
						if (dynamic_cast<Sketch*>(sO) != nullptr)
						{
							sO->isVisible = true;
							currentAppInstance->GetMenuFlag()->selectedObject1 = sO;
							currentAppInstance->GetMenuFlag()->editSketchDialogue = true;
						}
					}
					else if (sO->objectType == "Curve" && ImGui::MenuItem("Edit##tree"))
					{
						sO->isVisible = true;
						currentAppInstance->GetMenuFlag()->selectedObject1 = sO;

						strcpy_s(MenuFlags::extraCharArray, dynamic_cast<Curve3D*>(sO)->curveType.c_str());

						if (dynamic_cast<BSpline3D*>(sO) != nullptr)
						{
							for (int ctrlPointIdx = 0; ctrlPointIdx < dynamic_cast<BSpline3D*>(sO)->numControlPoints; ctrlPointIdx++)
							{
								MenuFlags::controlPoints[ctrlPointIdx] = dynamic_cast<BSpline3D*>(sO)->controlPoints[ctrlPointIdx];
							}

							MenuFlags::numControlPoints = dynamic_cast<BSpline3D*>(sO)->numControlPoints;
							MenuFlags::polynomialOrder = dynamic_cast<BSpline3D*>(sO)->polynomialOrder;

						}
						else if (dynamic_cast<Bezier3D*>(sO) != nullptr)
						{
							for (int ctrlPointIdx = 0; ctrlPointIdx < dynamic_cast<Bezier3D*>(sO)->numControlPoints; ctrlPointIdx++)
							{
								MenuFlags::controlPoints[ctrlPointIdx] = dynamic_cast<Bezier3D*>(sO)->controlPoints[ctrlPointIdx];
								MenuFlags::ctrPtsAsArray[ctrlPointIdx][0] = dynamic_cast<Bezier3D*>(sO)->controlPoints[ctrlPointIdx].pos.x;
								MenuFlags::ctrPtsAsArray[ctrlPointIdx][1] = dynamic_cast<Bezier3D*>(sO)->controlPoints[ctrlPointIdx].pos.y;
								MenuFlags::ctrPtsAsArray[ctrlPointIdx][2] = dynamic_cast<Bezier3D*>(sO)->controlPoints[ctrlPointIdx].pos.z;
							}

							MenuFlags::numControlPoints = dynamic_cast<Bezier3D*>(sO)->numControlPoints;
							MenuFlags::polynomialOrder = dynamic_cast<Bezier3D*>(sO)->polynomialOrder;
						}

						strcpy_s(MenuFlags::defaultObjectName, sO->displayName.c_str());
						currentAppInstance->GetMenuFlag()->editCurveDialogue = true;

					}

					if (sO->isVisible && ImGui::MenuItem("Focus View"))
					{
						glm::vec3 camOffset = currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition;
						camOffset -= currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetTarget();

						currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.SetTarget(sO->GetObjectPosition());

						if (dynamic_cast<Curve3D*>(sO) != nullptr)
						{
							Curve3D* focusCurve = dynamic_cast<Curve3D*>(sO);
							currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition = sO->GetObjectPosition() + focusCurve->GetValueAtParam(0.0f);
						}
						else if (dynamic_cast<Surface*>(sO) != nullptr)
						{
							Surface* focusSurface = dynamic_cast<Surface*>(sO);
							currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition = sO->GetObjectPosition() + focusSurface->GetValueAtParams(0.0f, 0.0f);
						}
						else
						{
							currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition = sO->GetObjectPosition() + camOffset;
						}
					}
					if (ImGui::MenuItem("Delete", nullptr, nullptr, (sO->canDelete())))
					{
						currentAppInstance->messageManager.ReceiveMessage({ "DELETE", sO->displayName });
					}
					ImGui::EndMenu();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

	}

	ImGui::End();
}

int AppGUI::CheckValidName(CAD_SCENE* currentScene, char* testName)
{
	std::string nameAsString = std::string(testName);
	if (nameAsString.empty())
	{
		return 100;
	}
	int nameIsUnused = 0;

	std::vector<std::string> currentNames;

	for (SceneObject* sO : currentScene->GetSceneObjects())
	{
		currentNames.push_back(sO->displayName);
	}
	for (SceneObject* sO : currentScene->GetDatumObjects())
	{
		currentNames.push_back(sO->displayName);
	}

	for (std::string usedName : currentNames)
	{
		if (testName == usedName)
		{
			nameIsUnused += 1;
			//return nameIsUnused;
		}
	}

	return nameIsUnused;
}

int AppGUI::SketchCheckValidName(Sketch* currentSketch, char* testName)
{
	int nameIsUnused = 0;

	std::vector<std::string> currentNames;

	for (SketchObject* sO : currentSketch->sketchObjects)
	{
		currentNames.push_back(sO->displayName);
	}

	for (std::string usedName : currentNames)
	{
		if (testName == usedName)
		{
			nameIsUnused += 1;
		}
	}

	return nameIsUnused;
}

void AppGUI::NewPointDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Point Dialogue");
	if (ImGui::BeginPopup("New Point Dialogue"))
	{
		ImGui::Text("Create a new datum point.");
		ImGui::Text("Enter the point in x, y, z coordinates.");

		bool isReady = AppGUI::PointPrompts(currentScene, true);

		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{
				Point* newPointObject = new Point(glm::vec3(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2]));
				newPointObject->displayName = std::string(MenuFlags::defaultObjectName);
				newPointObject->SetFlatColor({ 16, 16, 16 });
				currentScene->AddSceneObject(newPointObject);
				newPointObject = nullptr;
				
				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->newPointDialogue = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->newPointDialogue = false;
		}
		
		ImGui::EndPopup();
	}
}

void AppGUI::EditPointDialogue(CAD_SCENE* currentScene, Point* pointToEdit)
{
	ImGui::OpenPopup("Edit Point Dialogue");
	if (ImGui::BeginPopup("Edit Point Dialogue"))
	{
		ImGui::Text("Edit existing datum point.");
		ImGui::Text("Update the point in x, y, z coordinates.");
		
		bool isReady = AppGUI::PointPrompts(currentScene, false);

		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{
				pointToEdit->SetObjectPosition(glm::vec3(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2]));
				pointToEdit->displayName = std::string(MenuFlags::defaultObjectName);

				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->editPointDialogue = false;
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->editPointDialogue = false;
		}

		ImGui::EndPopup();
	}
}

bool AppGUI::PointPrompts(CAD_SCENE* currentScene, bool isNew)
{
	float* pos = MenuFlags::tempPoint1;
	if (ImGui::InputFloat3("Position", pos))
	{
		MenuFlags::tempPoint1[0] = pos[0];
		MenuFlags::tempPoint1[1] = pos[1];
		MenuFlags::tempPoint1[2] = pos[2];
	}

	ImGui::InputText("Edit Point Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);
	if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > (1 - (isNew * 1)))
	{
		ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
	}

	return (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) <= (1 - (isNew * 1)));
}

void AppGUI::NewAxisDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Axis Dialogue");
	if (ImGui::BeginPopup("New Axis Dialogue"))
	{
		ImGui::Text("Create a new datum axis.");
		ImGui::Text("Enter two points in x, y, z coordinates.");

		bool isReady = AppGUI::AxisPrompts(currentScene, true);

		if (ImGui::Button("Confirm"))
		{
			if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) == 0)
			{
				Axis* newAxisObject = new Axis(glm::vec3{ MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2]}, glm::vec3{ MenuFlags::tempPoint2[0], MenuFlags::tempPoint2[1], MenuFlags::tempPoint2[2] }, false);
				newAxisObject->displayName = std::string(MenuFlags::defaultObjectName);
				newAxisObject->SetFlatColor({ 196, 196, 196 });
				currentScene->AddSceneObject(newAxisObject);
				newAxisObject = nullptr;

				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->newAxisDialogue = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->newAxisDialogue = false;
		}

		ImGui::EndPopup();
	}
}

void AppGUI::EditAxisDialogue(CAD_SCENE* currentScene, Axis* axisToEdit)
{
	ImGui::OpenPopup("Edit Axis Dialogue");

	if (ImGui::BeginPopup("Edit Axis Dialogue"))
	{
		ImGui::Text("Update the axis with a new pair");
		ImGui::Text("of points in x, y, z coordinates.");
		
		bool isReady = AppGUI::AxisPrompts(currentScene, false);

		if (ImGui::Button("Confirm"))
		{
			if ((AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) <= 1) && (isReady))
			{
				axisToEdit->ChangeWithPoints(glm::vec3(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2]), glm::vec3(MenuFlags::tempPoint2[0], MenuFlags::tempPoint2[1], MenuFlags::tempPoint2[2]), MenuFlags::isReversed);
				axisToEdit->displayName = std::string(MenuFlags::defaultObjectName);

				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->editAxisDialogue = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();
			currentScene->GetParentApplication()->GetMenuFlag()->editAxisDialogue = false;
		}

		ImGui::EndPopup();
	}
}

bool AppGUI::AxisPrompts(CAD_SCENE* currentScene, bool isNew)
{
	float* p1 = MenuFlags::tempPoint1;
	float* p2 = MenuFlags::tempPoint2;


	if (ImGui::InputFloat3("Point 1", p1))
	{
		MenuFlags::tempPoint1[0] = p1[0];
		MenuFlags::tempPoint1[1] = p1[1];
		MenuFlags::tempPoint1[2] = p1[2];
	}

	if (ImGui::InputFloat3("Point 2", p2))
	{
		MenuFlags::tempPoint2[0] = p2[0];
		MenuFlags::tempPoint2[1] = p2[1];
		MenuFlags::tempPoint2[2] = p2[2];
	}

	glm::vec3 point1 = glm::vec3(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2]);
	glm::vec3 point2 = glm::vec3(MenuFlags::tempPoint2[0], MenuFlags::tempPoint2[1], MenuFlags::tempPoint2[2]);

	ImGui::Checkbox("Reversed", &MenuFlags::isReversed);

	ImGui::InputText("Edit Axis Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);
	if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > (1 - (isNew * 1)))
	{
		ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
	}
	if (point1 - point2 == glm::vec3(0.0f))
	{
		ImGui::TextColored(WARNING_COLOR, "Points cannot coincide!");
	}
	return (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) <= (1 - (isNew * 1)) && (point1 - point2 != glm::vec3(0.0f)));
}

void AppGUI::NewPlaneDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Plane Dialogue");
	if (ImGui::BeginPopup("New Plane Dialogue"))
	{
		ImGui::Text("Create a new datum plane.");
		ImGui::Text("Enter a point in x, y, z coordinates.");
		
		bool isReady = AppGUI::PlanePrompts(currentScene, true);

		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{
				Plane* newPlaneObject = new Plane({MenuFlags::tempPoint2[0], MenuFlags::tempPoint2[1], MenuFlags::tempPoint2[2]});
				newPlaneObject->SetObjectPosition({ MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2] });
				newPlaneObject->displayName = std::string(MenuFlags::defaultObjectName);
				currentScene->AddSceneObject(newPlaneObject);
				newPlaneObject = nullptr;

				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->newPlaneDialogue = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->newPlaneDialogue = false;
		}

		ImGui::EndPopup();
	}
}

void AppGUI::EditPlaneDialogue(CAD_SCENE* currentScene, Plane* planeToEdit)
{
	ImGui::OpenPopup("Edit Plane Dialogue");

	if (ImGui::BeginPopup("Edit Plane Dialogue"))
	{
		ImGui::Text("Update the plane with a new triplet");
		ImGui::Text("of points in x, y, z coordinates.");

		bool isReady = AppGUI::PlanePrompts(currentScene, false);
		
		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{
				glm::vec3 point1 = glm::vec3(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2]);
				glm::vec3 point2 = glm::vec3(MenuFlags::tempPoint2[0], MenuFlags::tempPoint2[1], MenuFlags::tempPoint2[2]);
				glm::vec3 point3 = glm::vec3(MenuFlags::tempPoint3[0], MenuFlags::tempPoint3[1], MenuFlags::tempPoint3[2]);

				planeToEdit->ChangeWithPoints(point1, point2, point3, MenuFlags::isReversed);

				planeToEdit->displayName = std::string(MenuFlags::defaultObjectName);

				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->editPlaneDialogue = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();
			currentScene->GetParentApplication()->GetMenuFlag()->editPlaneDialogue = false;
		}

		ImGui::EndPopup();
	}
}

bool AppGUI::PlanePrompts(CAD_SCENE* currentScene, bool isNew)
{
	if (!isNew)
	{
		float* p1 = MenuFlags::tempPoint1;
		float* p2 = MenuFlags::tempPoint2;
		float* p3 = MenuFlags::tempPoint3;


		if (ImGui::InputFloat3("Point 1", p1))
		{
			MenuFlags::tempPoint1[0] = p1[0];
			MenuFlags::tempPoint1[1] = p1[1];
			MenuFlags::tempPoint1[2] = p1[2];
		}

		if (ImGui::InputFloat3("Point 2", p2))
		{
			MenuFlags::tempPoint2[0] = p2[0];
			MenuFlags::tempPoint2[1] = p2[1];
			MenuFlags::tempPoint2[2] = p2[2];
		}

		if (ImGui::InputFloat3("Point 3", p3))
		{
			MenuFlags::tempPoint3[0] = p3[0];
			MenuFlags::tempPoint3[1] = p3[1];
			MenuFlags::tempPoint3[2] = p3[2];
		}

		ImGui::Text("The right axis of the plane points from");
		ImGui::Text("Point 1 to Point 2, unless `Flip Normal' is selected.");

		glm::vec3 a = glm::vec3(p2[0] - p1[0], p2[1] - p2[1], p2[2] - p1[2]);
		glm::vec3 b = glm::vec3(p3[0] - p1[0], p3[1] - p3[1], p3[2] - p1[2]);

		glm::vec3 tempNorm = glm::cross(glm::normalize(b), glm::normalize(a));

		if (&MenuFlags::isReversed)
		{
			tempNorm = tempNorm * -1.0f;
		}

		ImGui::TextColored({ 0.0f, 1.0f, 1.0f, 1.0f }, "Calculated Normal Vector:");
		ImGui::SameLine(); ImGui::Text("%g, %g, %g", tempNorm.x, tempNorm.y, tempNorm.z);

		glm::vec3 point1 = glm::vec3(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1], MenuFlags::tempPoint1[2]);
		glm::vec3 point2 = glm::vec3(MenuFlags::tempPoint2[0], MenuFlags::tempPoint2[1], MenuFlags::tempPoint2[2]);
		glm::vec3 point3 = glm::vec3(MenuFlags::tempPoint3[0], MenuFlags::tempPoint3[1], MenuFlags::tempPoint3[2]);

		ImGui::Checkbox("Flip Normal", &MenuFlags::isReversed);

		ImGui::InputText("Edit Plane Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);

		if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > (1 - (isNew * 1)))
		{
			ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
		}

		bool coincident = (point1 - point2 == glm::vec3(0.0f)) ||
			(point2 - point3 == glm::vec3(0.0f)) ||
			(point1 - point3 == glm::vec3(0.0f));

		bool colinear = (glm::normalize(point1 - point2) == glm::normalize(point2 - point3) ||
			glm::normalize(point1 - point2) == -glm::normalize(point2 - point3));

		if (coincident)
		{
			ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Points cannot coincide!");
		}
		if (colinear)
		{
			ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Points cannot be colinear!");
		}
		return (!coincident && !colinear && (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) <= (1 - (isNew * 1))));
	}
	else
	{
		float* pos1 = MenuFlags::tempPoint1;
		if (ImGui::InputFloat3("Point 1", pos1))
		{
			MenuFlags::tempPoint1[0] = pos1[0];
			MenuFlags::tempPoint1[1] = pos1[1];
			MenuFlags::tempPoint1[2] = pos1[2];
		};
		ImGui::Text("Enter a normal vector in x, y, z components.");
		float* norm = MenuFlags::tempPoint2;
		if (ImGui::InputFloat3("Normal Vector", norm))
		{
			MenuFlags::tempPoint2[0] = norm[0];
			MenuFlags::tempPoint2[1] = norm[1];
			MenuFlags::tempPoint2[2] = norm[2];
		};
		ImGui::InputText("Plane Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);
		if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > (1 - (isNew * 1)))
		{
			ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
		}
		if (norm[0] == norm[1] == norm[2] == 0.0f)
		{
			ImGui::TextColored({ 1.0f, 1.0f, 0, 1.0f }, "Invalid Normal Vector!");
		}

		return ((AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) <= (1 - (isNew * 1))) && !(norm[0] == norm[1] == norm[2] == 0.0f));
	}
	
}

void AppGUI::NewCurveDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Curve Dialogue");
	if (ImGui::BeginPopup("New Curve Dialogue"))
	{
		ImGui::Text("Create a new 3D curve.");
		ImGui::Text("Select the curve type:");

		bool isReady = AppGUI::CurvePrompts(currentScene, true);
		std::string curveType = std::string(MenuFlags::extraCharArray);

		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{
				if (curveType == "Line" && !isReady)
				{
					//ADD 3D LINE
					//Line3D* newLine = new Line3D();
					//newLine->SetControlPoints(MenuFlags::controlPoints);
					AppGUI::ResetDefaultValues();
					currentScene->GetParentApplication()->GetMenuFlag()->newCurveDialogue = false;
				}
				else if (curveType == "B-Spline")
				{
					BSpline3D* newBSpline = new BSpline3D();
					
					newBSpline->displayName = std::string(MenuFlags::defaultObjectName);
					newBSpline->SetControlPoints(MenuFlags::numControlPoints, MenuFlags::polynomialOrder, MenuFlags::controlPoints);
					currentScene->AddSceneObject(newBSpline);

					AppGUI::ResetDefaultValues();
					currentScene->GetParentApplication()->GetMenuFlag()->newCurveDialogue = false;
				}
				else if (curveType == "Bezier")
				{
					Bezier3D* newBezier = new Bezier3D();
					newBezier->SetControlPoints(MenuFlags::numControlPoints, MenuFlags::controlPoints);
					newBezier->displayName = std::string(MenuFlags::defaultObjectName);

					currentScene->AddSceneObject(newBezier);
					newBezier = nullptr;

					AppGUI::ResetDefaultValues();
					currentScene->GetParentApplication()->GetMenuFlag()->newCurveDialogue = false;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();
			
			currentScene->GetParentApplication()->GetMenuFlag()->newCurveDialogue = false;
		}
			
		ImGui::EndPopup();
	}
}

void AppGUI::EditCurveDialogue(CAD_SCENE* currentScene, Curve3D* curveToEdit)
{
	ImGui::OpenPopup("Edit Curve Dialogue");

	if (ImGui::BeginPopup("Edit Curve Dialogue"))
	{
		ImGui::Text("Edit an existing 3D curve.");
		ImGui::Text(("Curve Type is: " + curveToEdit->curveType).c_str());

		bool isReady = AppGUI::CurvePrompts(currentScene, false);
		std::string curveType = std::string(MenuFlags::extraCharArray);


		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{
				if (curveType == "B-Spline")
				{
					BSpline3D* newBSpline = new BSpline3D();

					newBSpline->displayName = std::string(MenuFlags::defaultObjectName);
					newBSpline->SetControlPoints(MenuFlags::numControlPoints, MenuFlags::polynomialOrder, MenuFlags::controlPoints);

					AppGUI::ResetDefaultValues();
					currentScene->GetParentApplication()->GetMenuFlag()->newCurveDialogue = false;

					currentScene->UpToDate = false;
				}
				else if (curveType == "Bezier")
				{
					
					((Bezier3D*)curveToEdit)->SetControlPoints(MenuFlags::numControlPoints, MenuFlags::controlPoints);
					((Bezier3D*)curveToEdit)->displayName = std::string(MenuFlags::defaultObjectName);

					AppGUI::ResetDefaultValues();
					currentScene->GetParentApplication()->GetMenuFlag()->editCurveDialogue = false;

					currentScene->UpToDate = false;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->editCurveDialogue = false;
		}

		ImGui::EndPopup();
	}

}

bool AppGUI::CurvePrompts(CAD_SCENE* currentScene, bool isNew)
{
	if (isNew)
	{
		std::string curveType = "-";
		if (!std::string(MenuFlags::extraCharArray).empty())
		{
			curveType = std::string(MenuFlags::extraCharArray);
		}

		if (ImGui::BeginCombo("Curve Type", curveType.c_str()))
		{
			if (ImGui::Selectable("Line"))
			{
				strcpy_s(MenuFlags::extraCharArray, "Line");
			}
			if (ImGui::Selectable("B-Spline"))
			{
				strcpy_s(MenuFlags::extraCharArray, "B-Spline");
			}
			if (ImGui::Selectable("Bezier"))
			{
				strcpy_s(MenuFlags::extraCharArray, "Bezier");
			}
			ImGui::EndCombo();
		}

		if (curveType == "Line")
		{
			ImGui::Text("Input two points, a start point");
			ImGui::Text("and an end point");
		}
		else if (curveType == "B-Spline")
		{
			ImGui::Text("Choose the number of control points.");
			ImGui::Text("(maxium 16)");
			ImGui::InputInt("Number of Points", &MenuFlags::numControlPoints, 1, 0);
			if (MenuFlags::numControlPoints > MAX_CTRL_PTS)
			{
				MenuFlags::numControlPoints = MAX_CTRL_PTS;
			}
			if (MenuFlags::numControlPoints < 2)
			{
				MenuFlags::numControlPoints = 2;
			}
			ImGui::Text("Choose the polynomial order of the curve.");
			ImGui::InputInt("Polynomial Order", &MenuFlags::polynomialOrder, 1, 0);
			if (MenuFlags::polynomialOrder > MenuFlags::numControlPoints - 1)
			{
				MenuFlags::polynomialOrder = MenuFlags::numControlPoints - 1;
			}
			if (MenuFlags::numControlPoints < 2)
			{
				MenuFlags::numControlPoints = 2;
			}
		}
		else if (curveType == "Bezier")
		{
			ImGui::Text("Choose the number of control points.");
			ImGui::Text("(maxium 16)");
			ImGui::InputInt("Number of Points", &MenuFlags::numControlPoints, 1, 0);
			if (MenuFlags::numControlPoints > MAX_CTRL_PTS)
			{
				MenuFlags::numControlPoints = MAX_CTRL_PTS;
			}
			if (MenuFlags::numControlPoints < 2)
			{
				MenuFlags::numControlPoints = 2;
			}
		}

		for (int controlPointIdx = 0; controlPointIdx < MenuFlags::numControlPoints; controlPointIdx++)
		{
			Point3D* point = &MenuFlags::controlPoints[controlPointIdx];
			float pArr[3] = { point->pos.x, point->pos.y, point->pos.z };
			std::string displayText = "Control Point ";
			displayText.append(std::to_string(controlPointIdx + 1));
			if (ImGui::InputFloat3(displayText.c_str(), MenuFlags::ctrPtsAsArray[controlPointIdx]))
			{
				point->pos.x = MenuFlags::ctrPtsAsArray[controlPointIdx][0];
				point->pos.y = MenuFlags::ctrPtsAsArray[controlPointIdx][1];
				point->pos.z = MenuFlags::ctrPtsAsArray[controlPointIdx][2];
			}
		}

		bool coincident = (curveType == "Line") && (MenuFlags::controlPoints[0].pos == MenuFlags::controlPoints[1].pos);
		coincident = (coincident) || (MenuFlags::numControlPoints == 2 && MenuFlags::controlPoints[0].pos == MenuFlags::controlPoints[1].pos);

		ImGui::InputText("Curve Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);

		if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > (1 - (isNew * 1)))
		{
			ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
		}

		if (coincident)
		{
			ImGui::TextColored(WARNING_COLOR, "Points cannot be coincident!");
		}

		if (curveType == "-")
		{
			ImGui::TextColored(WARNING_COLOR, "Select a curve type!");
		}

		return (!coincident) && (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) <= (1 - (isNew * 1)));
	}

	else
	{
		std::string curveType = "";
		if (!std::string(MenuFlags::extraCharArray).empty())
		{
			curveType = std::string(MenuFlags::extraCharArray);
		}

		if (curveType == "B-Spline")
		{
			ImGui::Text("Choose the number of control points.");
			ImGui::Text("(maxium 16)");
			ImGui::InputInt("Number of Points", &MenuFlags::numControlPoints, 1, 0);
			if (MenuFlags::numControlPoints > MAX_CTRL_PTS)
			{
				MenuFlags::numControlPoints = MAX_CTRL_PTS;
			}
			if (MenuFlags::numControlPoints < 2)
			{
				MenuFlags::numControlPoints = 2;
			}
			ImGui::Text("Choose the polynomial order of the curve.");
			ImGui::InputInt("Polynomial Order", &MenuFlags::polynomialOrder, 1, 0);
			if (MenuFlags::polynomialOrder > MenuFlags::numControlPoints - 1)
			{
				MenuFlags::polynomialOrder = MenuFlags::numControlPoints - 1;
			}
			if (MenuFlags::numControlPoints < 2)
			{
				MenuFlags::numControlPoints = 2;
			}
		}
		else if (curveType == "Bezier")
		{
			ImGui::Text("Choose the number of control points.");
			ImGui::Text("(maxium 16)");
			ImGui::InputInt("Number of Points", &MenuFlags::numControlPoints, 1, 0);
			if (MenuFlags::numControlPoints > MAX_CTRL_PTS)
			{
				MenuFlags::numControlPoints = MAX_CTRL_PTS;
			}
			if (MenuFlags::numControlPoints < 2)
			{
				MenuFlags::numControlPoints = 2;
			}
		}
		for (int controlPointIdx = 0; controlPointIdx < MenuFlags::numControlPoints; controlPointIdx++)
		{
			Point3D* point = &MenuFlags::controlPoints[controlPointIdx];
			float pArr[3] = { point->pos.x, point->pos.y, point->pos.z };
			std::string displayText = "Control Point ";
			displayText.append(std::to_string(controlPointIdx + 1));
			if (ImGui::InputFloat3(displayText.c_str(), MenuFlags::ctrPtsAsArray[controlPointIdx]))
			{
				point->pos.x = MenuFlags::ctrPtsAsArray[controlPointIdx][0];
				point->pos.y = MenuFlags::ctrPtsAsArray[controlPointIdx][1];
				point->pos.z = MenuFlags::ctrPtsAsArray[controlPointIdx][2];
			}
		}

		bool coincident = (MenuFlags::numControlPoints == 2 && MenuFlags::controlPoints[0].pos == MenuFlags::controlPoints[1].pos);

		ImGui::InputText("Curve Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);

		if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > (1 - (isNew * 1)))
		{
			ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
		}

		if (coincident)
		{
			ImGui::TextColored(WARNING_COLOR, "Points cannot be coincident!");
		}

		return (!coincident) && (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) <= (1 - (isNew * 1)));
	}
}

void AppGUI::NewSketchPointDialogue(CAD_SCENE* currentScene, Sketch* currentSketch)
{
	ImGui::OpenPopup("New Sketch Point Dialogue");
	if (ImGui::BeginPopup("New Sketch Point Dialogue"))
	{
		ImGui::Text("Create a new point.");
		ImGui::Text("Enter the point in x, y coordinates.");

		bool isReady = AppGUI::PointPrompts2D(currentSketch, true);

		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{
				currentSketch->AddPoint(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1]);

				AppGUI::ResetDefaultValues();

				MenuFlags::selectedObject1 = (SceneObject*)currentSketch;

				currentScene->GetParentApplication()->GetMenuFlag()->newSketchPointDialogue = false;
				currentScene->GetParentApplication()->GetMenuFlag()->inSketchMode = true;
				currentScene->UpToDate = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			MenuFlags::selectedObject1 = (SceneObject*)currentSketch;

			currentScene->GetParentApplication()->GetMenuFlag()->newSketchPointDialogue = false;
			currentScene->GetParentApplication()->GetMenuFlag()->inSketchMode = true;
		}

		ImGui::EndPopup();
	}
}

void AppGUI::EditSketchPointDialogue(CAD_SCENE* currentScene, Sketch* currentSketch, SketchPoint* pointToEdit)
{
	ImGui::OpenPopup("Edit Sketch Point Dialogue");
	if (ImGui::BeginPopup("Edit Sketch Point Dialogue"))
	{
		ImGui::Text("Edit an existing sketch point.");
		ImGui::Text("Enter the point in x, y coordinates.");

		bool isReady = AppGUI::PointPrompts2D(currentSketch, false);

		if (ImGui::Button("Confirm##SKETCHPOINTCONFIRM"))
		{
			if (isReady)
			{
				pointToEdit->MovePoint(MenuFlags::tempPoint1[0], MenuFlags::tempPoint1[1]);
				pointToEdit->displayName = std::string(MenuFlags::defaultObjectName);

				AppGUI::ResetDefaultValues();

				MenuFlags::selectedObject1 = (SceneObject*)currentSketch;

				currentScene->GetParentApplication()->GetMenuFlag()->editSketchPointDialogue = false;
				currentScene->GetParentApplication()->GetMenuFlag()->inSketchMode = true;
				currentScene->UpToDate = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##SKETCHPOINTCANCEL"))
		{
			AppGUI::ResetDefaultValues();

			MenuFlags::selectedObject1 = (SceneObject*)currentScene;

			currentScene->GetParentApplication()->GetMenuFlag()->editSketchPointDialogue = false;
			currentScene->GetParentApplication()->GetMenuFlag()->inSketchMode = true;
		}

		ImGui::EndPopup();
	}
}

bool AppGUI::PointPrompts2D(Sketch* currentSketch, bool isNew)
{
	float* pos = MenuFlags::tempPoint1;
	if (ImGui::InputFloat2("Position", pos))
	{
		MenuFlags::tempPoint1[0] = pos[0];
		MenuFlags::tempPoint1[1] = pos[1];
	}

	ImGui::InputText("Edit Point Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);
	if (AppGUI::SketchCheckValidName(currentSketch, MenuFlags::defaultObjectName) > (1 - (isNew * 1)))
	{
		ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
	}

	return (AppGUI::SketchCheckValidName(currentSketch, MenuFlags::defaultObjectName) <= (1 - (isNew * 1)));
}

void AppGUI::NewSketchDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Sketch Dialogue");
	if (ImGui::BeginPopup("New Sketch Dialogue"))
	{
		ImGui::Text("Select a plane for the sketch.");
		std::string previewText = "-";
		if (MenuFlags::selectedObject1)
		{
			previewText = MenuFlags::selectedObject1->displayName;
		}
		if (ImGui::BeginCombo("Sketch Plane", previewText.c_str()))
		{
			for (SceneObject* sO : currentScene->GetDatumObjects())
			{
				if (sO->objectType == "Plane")
				{
					if (ImGui::Selectable(sO->displayName.c_str()))
					{
						MenuFlags::selectedObject1 = sO;
					}
				}
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("Sketch Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);
		if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > 0)
		{
			ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
		}
		if (!MenuFlags::selectedObject1)
		{
			ImGui::TextColored(WARNING_COLOR, "No plane selected!");
		}
		ImGui::Checkbox("Edit on Create", &currentScene->GetParentApplication()->GetMenuFlag()->editOnCreate);
		if (ImGui::Button("Confirm"))
		{
			if ((AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) == 0) && MenuFlags::selectedObject1)
			{
				Sketch* newSketchObject = new Sketch((Plane*)MenuFlags::selectedObject1);
				newSketchObject->displayName = std::string(MenuFlags::defaultObjectName);
				
				
				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->newSketchDialogue = false;

				if (currentScene->GetParentApplication()->GetMenuFlag()->editOnCreate)
				{
					currentScene->GetParentApplication()->GetMenuFlag()->inSketchMode = true;
					currentScene->GetParentApplication()->currentSketch = newSketchObject;
				}

				currentScene->AddSceneObject(newSketchObject);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->newSketchDialogue = false;
		}

		ImGui::EndPopup();
	}
}

void AppGUI::EditSketchDialogue(CAD_SCENE* currentScene, Sketch* sketchToEdit)
{
	currentScene->GetParentApplication()->GetMenuFlag()->editSketchDialogue = false;
}

void AppGUI::NewSketchLineDialogue(CAD_SCENE* currentScene, Sketch* currentSketch)
{
	currentScene->GetParentApplication()->GetMenuFlag()->newSketchLineDialogue = false;
}
void AppGUI::EditSketchLineDialogue(CAD_SCENE* currentScene, Sketch* currentSketch, SketchLine* lineToEdit)
{
	currentScene->GetParentApplication()->GetMenuFlag()->newSketchLineDialogue = false;
}
bool AppGUI::LinePrompts2D(Sketch* currentSketch, bool isNew)
{
	return false;
}

void AppGUI::NewSketchCurveDialogue(CAD_SCENE* currentScene, Sketch* currentSketch)
{
	currentScene->GetParentApplication()->GetMenuFlag()->newSketchCurveDialogue = false;
}
void AppGUI::EditSketchCurveDialogue(CAD_SCENE* currentScene, Sketch* currentSketch, SketchCurve* curveToEdit)
{
	currentScene->GetParentApplication()->GetMenuFlag()->newSketchCurveDialogue = false;
}
bool AppGUI::CurvePrompts2D(Sketch* currentSketch, bool isNew)
{
	return false;
}

void AppGUI::NewSurfaceDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Surface Dialogue");
	if (ImGui::BeginPopup("New Surface Dialogue"))
	{
		ImGui::Text("Select a surface type.");
		std::string previewText = std::string(MenuFlags::extraCharArray);
		if (previewText.empty())
		{
			previewText = "-";
		}
		if (ImGui::BeginCombo("Surface Type", previewText.c_str()))
		{
			if (ImGui::Selectable("Ruled##RULEDSELECTABLE"))
			{
				strcpy_s(MenuFlags::extraCharArray, "Ruled");
			}
			if (ImGui::Selectable("-"))
			{
				strcpy_s(MenuFlags::extraCharArray, "-");
			}

			ImGui::EndCombo();
		}

		bool isReady = true;

		if (previewText == "-")
		{
			ImGui::TextColored(WARNING_COLOR, "No surface type selected!");
			isReady = false;
		}
		
		if (ImGui::Button("Confirm"))
		{
			if (isReady)
			{

				currentScene->GetParentApplication()->GetMenuFlag()->newSurfaceDialogue = false;

				if (previewText == "Ruled")
				{
					currentScene->GetParentApplication()->GetMenuFlag()->newRuledDialogue = true;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->newSurfaceDialogue = false;
		}

		ImGui::EndPopup();
	}
}

void AppGUI::NewRuledDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Ruled Surface Dialogue");
	if (ImGui::BeginPopup("New Ruled Surface Dialogue"))
	{
		ImGui::Text("Select two curves for the surface.");
		std::string previewText1 = "-";
		std::string previewText2 = "-";
		if (MenuFlags::selectedObject1)
		{
			previewText1 = MenuFlags::selectedObject1->displayName;
		}
		if (MenuFlags::selectedObject2)
		{
			previewText2 = MenuFlags::selectedObject2->displayName;
		}
		if (ImGui::BeginCombo("Curve 1##FIRSTCURVE", previewText1.c_str()))
		{
			for (SceneObject* sO : currentScene->GetSceneObjects())
			{
				if (sO->objectType == "Curve" )
				{
					if (ImGui::Selectable(sO->displayName.c_str()))
					{
						MenuFlags::selectedObject1 = sO;
					}
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Curve 2##FIRSTCURVE", previewText2.c_str()))
		{
			for (SceneObject* sO : currentScene->GetSceneObjects())
			{
				if (sO->objectType == "Curve")
				{
					if (ImGui::Selectable(sO->displayName.c_str()))
					{
						MenuFlags::selectedObject2 = sO;
					}
				}
			}
			ImGui::EndCombo();
		}
		bool isReady = true;

		ImGui::InputText("Surface Name", MenuFlags::defaultObjectName, MAX_NAME_LENGTH * sizeof(char), 0, 0, 0);
		if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) > 0)
		{
			ImGui::TextColored(WARNING_COLOR, INVALID_NAME_WARNING);
			isReady = false;
		}
		if (!MenuFlags::selectedObject1 || !MenuFlags::selectedObject2)
		{
			ImGui::TextColored(WARNING_COLOR, "Not enough curves selected!");
			isReady = false;
		}

		if (ImGui::Button("Confirm"))
		{
			if ((AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) == 0) && isReady)
			{

				currentScene->GetParentApplication()->GetMenuFlag()->newRuledDialogue = false;
				
				Ruled* newRuled = new Ruled();
				newRuled->SetCurves((Curve3D*)MenuFlags::selectedObject1, (Curve3D*)::MenuFlags::selectedObject2);
				newRuled->displayName = std::string(MenuFlags::defaultObjectName);
				currentScene->AddSceneObject(newRuled);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->newRuledDialogue = false;
		}

		ImGui::EndPopup();
	}
}

void AppGUI::ResetDefaultValues()
{
	MenuFlags::tempPoint1[0] = 0.0f;
	MenuFlags::tempPoint1[1] = 0.0f;
	MenuFlags::tempPoint1[2] = 0.0f;

	MenuFlags::tempPoint2[0] = 0.0f;
	MenuFlags::tempPoint2[1] = 0.0f;
	MenuFlags::tempPoint2[2] = 0.0f;

	MenuFlags::tempPoint3[0] = 0.0f;
	MenuFlags::tempPoint3[1] = 0.0f;
	MenuFlags::tempPoint3[2] = 0.0f;

	MenuFlags::isReversed = false;

	MenuFlags::numControlPoints = 2;
	MenuFlags::polynomialOrder = 1;

	for (int i = 0; i < MAX_NAME_LENGTH; i++)
	{
		MenuFlags::defaultObjectName[i] = '\0';
		MenuFlags::extraCharArray[i] = '\0';
	}

	for (int i = 0; i < MAX_CTRL_PTS; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			MenuFlags::ctrPtsAsArray[i][j] = 0.0f;
		}
		MenuFlags::controlPoints[i] = Point3D();
		MenuFlags::controlPoints2D[i] = SketchPoint();
	}
}

