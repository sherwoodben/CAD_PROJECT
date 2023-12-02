#pragma once
#include "SceneObject.h"
#include "SketchObjects.h"

#include <vector>
#include "Plane.h"

#define MAX_CTRL_PTS 16

class Sketch : public SceneObject
{
public:
	Sketch(Plane* parentPlane) { sketchPlane = parentPlane; this->InitSketch(); };
	~Sketch() { this->DeleteObject(); };

	void PassShaderData(Shader* shader);
	void RenderObject(Shader* sketchInPlaneShader);

	glm::mat4 GetModelMatrix() { return this->sketchPlane->GetModelMatrix(); };

	void RenderSketchObjects(Shader* shader);

	void AddPoint(float x, float y) { this->sketchObjects.push_back(new SketchPoint(x, y)); };
	void AddLine(SketchPoint p1, SketchPoint p2, bool reversed = false) { this->sketchObjects.push_back(new SketchLine(p1, p2, reversed)); };
	void AddCurve(SketchCurve* curve) { this->sketchObjects.push_back(curve); };

	void DeleteSketchObjects();

	void DeleteObject();

	glm::vec3 SketchToWorldPosition(float x, float y);

	std::vector<SketchObject*> sketchObjects;


private:
	Plane* sketchPlane = nullptr;



	unsigned int sketchFBO = 0;
	unsigned int sketchRenderedTexture;

	void InitSketch();
};

