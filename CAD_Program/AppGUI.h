#pragma once
#include "imgui.h"

class CAD_APP;
class CAD_SCENE;
class SceneObject;
class Point;
class Axis;
class Plane;
class Sketch;
struct Point3D;
class Curve3D;
class SketchObject;
class SketchPoint;
class SketchLine;
class SketchCurve;
class Surface;
class Ruled;

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
	bool newCurveDialogue = false;
	bool editCurveDialogue = false;

	bool newSketchDialogue = false;
	bool editSketchDialogue = false;
	bool inSketchMode = false;

	bool newSketchPointDialogue = false;
	bool editSketchPointDialogue = false;
	bool newSketchLineDialogue = false;
	bool editSketchLineDialogue = false;
	bool newSketchCurveDialogue = false;
	bool editSketchCurveDialogue = false;

	bool newSurfaceDialogue = false;

	bool newRuledDialogue = false;
	bool editRuledDialogue = false;

	bool editOnCreate = true;

	static float tempPoint1[];
	static float tempPoint2[];
	static float tempPoint3[];
	static char defaultObjectName[];
	static char extraCharArray[];

	static bool isReversed;

	static int numControlPoints;
	static int polynomialOrder;

	static SceneObject* selectedObject1;
	static SceneObject* selectedObject2;
	static SceneObject* selectedObject3;

	static SketchObject* selectedSketchObject1;
	static SketchObject* selectedSketchObject2;
	static SketchObject* selectedSketchObject3;

	static Point3D controlPoints[];
	static SketchPoint controlPoints2D[];

	static float ctrPtsAsArray[][3];
};

namespace AppGUI {
	void RenderMenuBar(float& yOffset, CAD_APP* currentAppInstance);

	void PreferencesMenu(CAD_APP* currentAppInstance);

	void CameraViewSubmenu(CAD_APP* currentAppInstance);

	void RenderSceneTree(int regionWidth, float yOffset, CAD_APP* currentAppInstance);

	void NewPointDialogue(CAD_SCENE* currentScene);
	void EditPointDialogue(CAD_SCENE* currentScene, Point* pointToEdit);
	bool PointPrompts(CAD_SCENE* currentScene, bool isNew);

	void NewAxisDialogue(CAD_SCENE* currentScene);
	void EditAxisDialogue(CAD_SCENE* currentScene, Axis* axisToEdit);
	bool AxisPrompts(CAD_SCENE* currentScene, bool isNew);

	void NewPlaneDialogue(CAD_SCENE* currentScene);
	void EditPlaneDialogue(CAD_SCENE* currentScene, Plane* planeToEdit);
	bool PlanePrompts(CAD_SCENE* currentScene, bool isNew);

	void NewSketchDialogue(CAD_SCENE* currentScene);
	void EditSketchDialogue(CAD_SCENE* currentScene, Sketch* sketchToEdit);

	void NewCurveDialogue(CAD_SCENE* currentScene);
	void EditCurveDialogue(CAD_SCENE* currentScene, Curve3D* curveToEdit);
	bool CurvePrompts(CAD_SCENE* currentScene, bool isNew);

	void NewSketchPointDialogue(CAD_SCENE* currentScene, Sketch* currentSketch);
	void EditSketchPointDialogue(CAD_SCENE* currentScene, Sketch* currentSketch, SketchPoint* pointToEdit);
	bool PointPrompts2D(Sketch* currentSketch, bool isNew);
	
	void NewSketchLineDialogue(CAD_SCENE* currentScene, Sketch* currentSketch);
	void EditSketchLineDialogue(CAD_SCENE* currentScene, Sketch* currentSketch, SketchLine* lineToEdit);
	bool LinePrompts2D(Sketch* currentSketch, bool isNew);
	
	void NewSketchCurveDialogue(CAD_SCENE* currentScene, Sketch* currentSketch);
	void EditSketchCurveDialogue(CAD_SCENE* currentScene, Sketch* currentSketch, SketchCurve* curveToEdit);
	bool CurvePrompts2D(Sketch* currentSketch, bool isNew);

	void NewSurfaceDialogue(CAD_SCENE* currentScene);
	
	void NewRuledDialogue(CAD_SCENE* currentScene);
	void EditRuledDialogue(CAD_SCENE* currentScene, Ruled* ruledToEdit);

	bool CurvePrompts2D(Sketch* currentSketch, bool isNew);


	int CheckValidName(CAD_SCENE* currentScene, char* testName);

	int SketchCheckValidName(Sketch* currentSketch, char* testName);

	void ResetDefaultValues();

}