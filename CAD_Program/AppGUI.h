#pragma once
#include "imgui.h"

class CAD_APP;
class CAD_SCENE;
class SceneObject;

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
	bool newAxisDialogue = false;
	bool newPlaneDialogue = false;
	bool newSketchDialogue = false;

	static float tempPoint1[];
	static float tempPoint2[];
	static float tempPoint3[];
	static char defaultObjectName[];
	static SceneObject* selectedObject1;
	static SceneObject* selectedObject2;
	static SceneObject* selectedObject3;
	
};

namespace AppGUI {
	void RenderMenuBar(float& yOffset, CAD_APP* currentAppInstance);

	void PreferencesMenu(CAD_APP* currentAppInstance);

	void CameraViewSubmenu(CAD_APP* currentAppInstance);

	void RenderSceneTree(int regionWidth, float yOffset, CAD_APP* currentAppInstance);

	void NewPointDialogue(CAD_SCENE* currentScene);

	void NewAxisDialogue(CAD_SCENE* currentScene);

	void NewPlaneDialogue(CAD_SCENE* currentScene);

	void NewSketchDialogue(CAD_SCENE* currentScene);

	bool CheckValidName(CAD_SCENE* currentScene, char* testName);

	void ResetDefaultValues();

}