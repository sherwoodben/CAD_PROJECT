#pragma once
#include "imgui.h"

class CAD_APP;
class CAD_SCENE;

//will eventualyl turn this into just flags
//instead of a bunch of bools, but this is
//fairly flexible for now
struct MenuFlags
{
	bool openFileMenu = false;
	bool saveMenu = false;
	bool saveAsMenu = false;

	bool preferencesMenu = false;

	bool newPointDialogue = false;

	static float tempPoint[];
	static char defaultObjectName[];
	
};

namespace AppGUI {
	void RenderMenuBar(float& yOffset, CAD_APP* currentAppInstance);

	void PreferencesMenu(CAD_APP* currentAppInstance);

	void CameraViewSubmenu(CAD_APP* currentAppInstance);

	void RenderSceneTree(int regionWidth, float yOffset, CAD_APP* currentAppInstance);

	void NewPointDialogue(CAD_SCENE* currentScene);

	bool CheckValidName(CAD_SCENE* currentScene, char* testName);
}