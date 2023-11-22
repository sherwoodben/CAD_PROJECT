#include "AppGUI.h"
#include "CAD_APP.h"
#include "CAD_SCENE.h"
#include "Point.h"

float MenuFlags::tempPoint[3] = {0.0f, 0.0f, 0.0f};
char MenuFlags::defaultObjectName[16] = "DEFAULT NAME";

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
				}
				if (ImGui::MenuItem("Plane"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "PLANE" });
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("New Feature/Object"))
			{
				if (ImGui::MenuItem("Sketch"))
				{
					currentAppInstance->messageManager.ReceiveMessage({ "NEW_OBJECT", "SKETCH" });
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
			if (currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState().cameraIsOrthographic &&
				ImGui::MenuItem("Toggle Perspective"))
			{
				currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.SetPerspectiveMode();
			}
			else if (!currentAppInstance->GetCurrentScene()->sceneState.SceneCamera.GetCameraState().cameraIsOrthographic &&
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
			ImGui::TextColored({ 128, 0, 128, 255 }, ("[" + sO->objectType + "]").c_str());
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
		float* pos = MenuFlags::tempPoint;
		if (ImGui::InputFloat3("Coordinates", pos))
		{
			MenuFlags::tempPoint[0] = pos[0];
			MenuFlags::tempPoint[1] = pos[1];
			MenuFlags::tempPoint[2] = pos[2];
		};
		if (ImGui::InputText("Point Name", MenuFlags::defaultObjectName, 16 * sizeof(char), 0, 0, 0 ))
		{

		}
		if (!AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
		{
			ImGui::TextColored({ 1.0f, 0, 0, 1.0f }, "Name is already used!");
		}
		if (ImGui::Button("Confirm"))
		{
			if (AppGUI::CheckValidName(currentScene, MenuFlags::defaultObjectName))
			{
				Point* newPointObject = new Point(glm::vec3(MenuFlags::tempPoint[0], MenuFlags::tempPoint[1], MenuFlags::tempPoint[2]));
				newPointObject->displayName = std::string(MenuFlags::defaultObjectName);
				currentScene->AddSceneObject(newPointObject);
				newPointObject = nullptr;
				MenuFlags::tempPoint[0] = 0.0f;
				MenuFlags::tempPoint[1] = 0.0f;
				MenuFlags::tempPoint[2] = 0.0f;

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

				currentScene->GetParentApplication()->GetMenuFlag()->newPointDialogue = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			MenuFlags::tempPoint[0] = 0.0f;
			MenuFlags::tempPoint[1] = 0.0f;
			MenuFlags::tempPoint[2] = 0.0f;

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

			currentScene->GetParentApplication()->GetMenuFlag()->newPointDialogue = false;
		}
		
		
		ImGui::EndPopup();
	}
}
