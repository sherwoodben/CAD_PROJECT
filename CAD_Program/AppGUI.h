#pragma once
#include "imgui.h"

class CAD_APP;
class CAD_SCENE;
class SceneObject;
class Point;
class Axis;
class Plane;
class Sketch;

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
	bool editPointDialogue = false;
	bool newAxisDialogue = false;
	bool editAxisDialogue = false;
	bool newPlaneDialogue = false;
	bool editPlaneDialogue = false;
	bool newSketchDialogue = false;
	bool editSketchDialogue = false;
	bool inSketchMode = false;

	bool newSketchPointDialogue = false;
	bool editSketchPointDialogue = false;
	bool newSketchLineDialogue = false;
	bool editSketchLineDialogue = false;
	bool newSketchCurveDialogue = false;
	bool editSketchCurveDialogue = false;

	bool editOnCreate = true;

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
	void EditPointDialogue(CAD_SCENE* currentScene, Point* pointToEdit);

	void NewAxisDialogue(CAD_SCENE* currentScene);
	void EditAxisDialogue(CAD_SCENE* currentScene, Axis* axisToEdit);

	void NewPlaneDialogue(CAD_SCENE* currentScene);
	void EditPlaneDialogue(CAD_SCENE* currentScene, Plane* planeToEdit);

	void NewSketchDialogue(CAD_SCENE* currentScene);
	void EditSketchDialogue(CAD_SCENE* currentScene, Sketch* sketchToEdit);

	//void NewSketchPointDialogue(Sketch* currentSketch);
	//void EditSketchPointDialogue(Sketch* currentSketch, SketchPoint* pointToEdit);
	//
	//void NewSketchLineDialogue(Sketch* currentSketch);
	//void EditSketchLineDialogue(Sketch* currentSketch, SketchLine* lineToEdit);
	//
	//void NewSketchCurveDialogue(Sketch* currentSketch);
	//void EditSketchCurveDialogue(Sketch* currentSketch, SketchCurve* curveToEdit);


	int CheckValidName(CAD_SCENE* currentScene, char* testName);

	int SketchCheckValidName(Sketch* currentSketch, char* testName);

	void ResetDefaultValues();

}