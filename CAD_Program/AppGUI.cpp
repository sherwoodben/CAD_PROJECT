#include "AppGUI.h"
#include "CAD_APP.h"
#include "CAD_SCENE.h"

float MenuFlags::tempPoint1[3] = { 0.0f, 0.0f, 0.0f };
float MenuFlags::tempPoint2[3] = {0.0f, 0.0f, 0.0f};
char MenuFlags::defaultObjectName[16] = "DEFAULT NAME";

SceneObject* MenuFlags::selectedObject1 = nullptr;
SceneObject* MenuFlags::selectedObject2 = nullptr;
SceneObject* MenuFlags::selectedObject3 = nullptr;

void AppGUI::RenderMenuBar(float& yOffset, CAD_APP* currentAppInstance)
{
	if (ImGui::BeginMainMenuBar())
	{
		//file
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{

			}
			if (ImGui::MenuItem("Open"))
			{

			}
			if (ImGui::MenuItem("Save"))
			{

			}
			if (ImGui::MenuItem("Save As"))
			{

			}
			if (ImGui::MenuItem("Quit"))
			{
				glfwSetWindowShouldClose(currentAppInstance->GetApplicationWindow(), GLFW_TRUE);
			}
			ImGui::EndMenu();
		}
		//edit
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::BeginMenu("New Datum"))
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
			if (ImGui::BeginMenu("New Feature/Object"))
			{
				if (ImGui::MenuItem("Sketch"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "SKETCH" });
					currentAppInstance->GetMenuFlag()->newSketchDialogue = true;
				}
				if (ImGui::MenuItem("Surface"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "SURFACE" });
				}
				if (ImGui::MenuItem("Solid"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "SOLID" });
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		//edit
		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Preferences"))
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
	
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Datum Objects"))
	{
		//list the objects
		for (SceneObject* sO : currentAppInstance->GetCurrentScene()->GetDatumObjects())
		{
			
			ImGui::PushID(sO->GetObjectVAOPointer());
			
			if (sO->isDefaultObject)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, {0.0f, 200.0f/255.0f, 200.0f/255.0f, 1.0f});
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
			ImGui::SetCursorPosX({ (float)regionWidth - 1.5f * ImGui::CalcTextSize("...")[0]});
			if (ImGui::BeginMenu("..."))
			{
				if (sO->isVisible && ImGui::MenuItem("Hide"))
				{
					sO->isVisible = false;
				}
				else if (!sO->isVisible && ImGui::MenuItem("Show"))
				{
					sO->isVisible = true;
				}
				if (sO->isVisible && ImGui::MenuItem("Focus View"))
				{
					glm::vec3 camOffset = currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition;
					camOffset -= currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetTarget();

					currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.SetTarget(sO->GetObjectPosition());
					
					currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition = sO->GetObjectPosition() + camOffset;
				}
				if (ImGui::MenuItem("Delete", nullptr, nullptr, !sO->isDefaultObject))
				{
					currentAppInstance->messageManager.ReceiveMessage({"DELETE", sO->displayName});
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
				if (sO->objectType == "Sketch" && ImGui::MenuItem("Edit"))
				{
					currentAppInstance->GetMenuFlag()->editSketchMenu = true;
					currentAppInstance->GetMenuFlag()->selectedObject1 = sO;
				}
				if (sO->isVisible && ImGui::MenuItem("Hide"))
				{
					sO->isVisible = false;
				}
				else if (!sO->isVisible && ImGui::MenuItem("Show"))
				{
					sO->isVisible = true;
				}
				if (sO->isVisible && ImGui::MenuItem("Focus View"))
				{
					glm::vec3 camOffset = currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition;
					camOffset -= currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetTarget();

					currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.SetTarget(sO->GetObjectPosition());

					currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState()->cameraPosition = sO->GetObjectPosition() + camOffset;
				}
				if (ImGui::MenuItem("Delete", nullptr, nullptr, !sO->isDefaultObject))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "DELETE", sO->displayName });
				}
				ImGui::EndMenu();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	

	ImGui::End();
}

bool AppGUI::CheckValidName(CAD_SCENE* currentScene, char* testName)
{
	bool nameIsUnused = true;

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
			nameIsUnused = false;
			return nameIsUnused;
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
		float* pos = MenuFlags::tempPoint1;
		if (ImGui::InputFloat3("Coordinates", pos))
		{
			MenuFlags::tempPoint1[0] = pos[0];
			MenuFlags::tempPoint1[1] = pos[1];
			MenuFlags::tempPoint1[2] = pos[2];
		};
		ImGui::InputText("Point Name", MenuFlags::defaultObjectName, 16 * sizeof(char), 0, 0, 0);
		if (!AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
		{
			ImGui::TextColored({ 1.0f, 0, 0, 1.0f }, "Name is already used!");
		}
		if (ImGui::Button("Confirm"))
		{
			if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
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

void AppGUI::NewAxisDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Axis Dialogue");
	if (ImGui::BeginPopup("New Axis Dialogue"))
	{
		ImGui::Text("Create a new datum axis.");
		ImGui::Text("Enter two points in x, y, z coordinates.");
		float* pos1 = MenuFlags::tempPoint1;
		float* pos2 = MenuFlags::tempPoint2;
		if (ImGui::InputFloat3("Point 1", pos1))
		{
			MenuFlags::tempPoint1[0] = pos1[0];
			MenuFlags::tempPoint1[1] = pos1[1];
			MenuFlags::tempPoint1[2] = pos1[2];
		};
		if (ImGui::InputFloat3("Point 2", pos2))
		{
			MenuFlags::tempPoint2[0] = pos2[0];
			MenuFlags::tempPoint2[1] = pos2[1];
			MenuFlags::tempPoint2[2] = pos2[2];
		};
		ImGui::InputText("Axis Name", MenuFlags::defaultObjectName, 16 * sizeof(char), 0, 0, 0);
		if (!AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
		{
			ImGui::TextColored({ 1.0f, 0, 0, 1.0f }, "Name is already used!");
		}
		if (ImGui::Button("Confirm"))
		{
			if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
			{
				Axis* newAxisObject = new Axis(glm::vec3{pos1[0], pos1[1], pos1[2]}, glm::vec3{ pos2[0], pos2[1], pos2[2] }, false);
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

void AppGUI::NewPlaneDialogue(CAD_SCENE* currentScene)
{
	ImGui::OpenPopup("New Plane Dialogue");
	if (ImGui::BeginPopup("New Plane Dialogue"))
	{
		ImGui::Text("Create a new datum plane.");
		ImGui::Text("Enter a point in x, y, z coordinates.");
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
		ImGui::InputText("Plane Name", MenuFlags::defaultObjectName, 16 * sizeof(char), 0, 0, 0);
		if (!AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
		{
			ImGui::TextColored({ 1.0f, 0, 0, 1.0f }, "Name is already used!");
		}
		if (ImGui::Button("Confirm"))
		{
			if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
			{
				Plane* newPlaneObject = new Plane({norm[0], norm[1], norm[2]});
				newPlaneObject->SetObjectPosition({ pos1[0], pos1[1], pos1[2] });
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

		ImGui::InputText("Sketch Name", MenuFlags::defaultObjectName, 16 * sizeof(char), 0, 0, 0);
		if (!AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
		{
			ImGui::TextColored({ 1.0f, 0, 0, 1.0f }, "Name is already used!");
		}
		if (!MenuFlags::selectedObject1)
		{
			ImGui::TextColored({ 1.0f, 0, 0, 1.0f }, "No plane selected!");
		}
		ImGui::Checkbox("Edit on Create", &currentScene->GetParentApplication()->GetMenuFlag()->editOnCreate);
		if (ImGui::Button("Confirm"))
		{
			if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName) && MenuFlags::selectedObject1)
			{
				Sketch* newSketchObject = new Sketch((Plane*)MenuFlags::selectedObject1);
				newSketchObject->displayName = std::string(MenuFlags::defaultObjectName);
				currentScene->AddSceneObject(newSketchObject);
				

				AppGUI::ResetDefaultValues();

				currentScene->GetParentApplication()->GetMenuFlag()->newSketchDialogue = false;

				if (currentScene->GetParentApplication()->GetMenuFlag()->editOnCreate)
				{
					currentScene->GetParentApplication()->GetMenuFlag()->editSketchMenu = true;
					currentScene->GetParentApplication()->GetMenuFlag()->selectedObject1 = (SceneObject*)newSketchObject;
				}

				newSketchObject = nullptr;
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

void AppGUI::EditSketchMenu(CAD_SCENE* currentScene, SceneObject* activeSketch)
{
	ImGui::OpenPopup("Edit Sketch Menu");
	if (ImGui::BeginPopup("Edit Sketch Menu"))
	{
		std::string displayText = "Editing Sketch: " + activeSketch->displayName;
		ImGui::Text(displayText.c_str());

		if (ImGui::Button("Exit Sketch Mode"))
		{
			AppGUI::ResetDefaultValues();

			currentScene->GetParentApplication()->GetMenuFlag()->editSketchMenu = false;
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

	MenuFlags::defaultObjectName[0] = 'D';
	MenuFlags::defaultObjectName[1] = 'E';
	MenuFlags::defaultObjectName[2] = 'F';
	MenuFlags::defaultObjectName[3] = 'A';
	MenuFlags::defaultObjectName[4] = 'U';
	MenuFlags::defaultObjectName[5] = 'L';
	MenuFlags::defaultObjectName[6] = 'T';
	MenuFlags::defaultObjectName[7] = ' ';
	MenuFlags::defaultObjectName[8] = 'N';
	MenuFlags::defaultObjectName[9] = 'A';
	MenuFlags::defaultObjectName[10] = 'M';
	MenuFlags::defaultObjectName[11] = 'E';
	MenuFlags::defaultObjectName[12] = '\0';
	MenuFlags::defaultObjectName[13] = '\0';
	MenuFlags::defaultObjectName[14] = '\0';
	MenuFlags::defaultObjectName[15] = '\0';

	MenuFlags::selectedObject1 = nullptr;
	MenuFlags::selectedObject2 = nullptr;
	MenuFlags::selectedObject3 = nullptr;
}

