#pragma once
#include "SceneObject.h"
#include "3DCurves.h"
#include "Sketch.h"
#include "SketchObjects.h"
#include "Axis.h"

#include <format>

#define SURFACE_QUALITY 33
#define BACK_FACE_COLOR glm::vec4(1.0f, 0.666f, 0.f, 1.0f)
#define FRONT_FACE_COLOR glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)
#define SURFACE_ALPHA 0.6f

class Surface : public SceneObject
{
public:
	Surface()
	{
		this->InitSurface();
	};

	~Surface() {};

	void InitSurface()
	{
		this->objectType = "Surface";
		this->SetIsTransparent(true);

		for (int idx = 0; idx < 3 * (SURFACE_QUALITY * SURFACE_QUALITY); idx++)
		{
			this->displayPoints[idx] = 0.0f;
		}

		if (this->indices != nullptr)
		{
			for (int y = 0; y < SURFACE_QUALITY; y++)
			{
				for (int x = 0; x < SURFACE_QUALITY; x++)
				{
					int quadIndex = x + ((SURFACE_QUALITY-1) * y);

					this->indices[(quadIndex * 6) + 0] = (x + 0) + (y * SURFACE_QUALITY);
					this->indices[(quadIndex * 6) + 1] = (x + 1) + (y * SURFACE_QUALITY);
					this->indices[(quadIndex * 6) + 2] = (x + 0) + ((y + 1) * SURFACE_QUALITY);

					this->indices[(quadIndex * 6) + 3] = (x + 0) + ((y + 1) * SURFACE_QUALITY);
					this->indices[(quadIndex * 6) + 4] = (x + 1) + (y * SURFACE_QUALITY);
					this->indices[(quadIndex * 6) + 5] = (x + 1) + ((y + 1) * SURFACE_QUALITY);
				}
			}
		}

		this->SetUpBuffers();

	};

	void Update()
	{
		this->Init();
		this->CalculateDisplayPoints();
	};

	virtual void Init() = 0;

	void SetUpBuffers()
	{
		glGenVertexArrays(1, this->GetObjectVAOPointer());
		glGenBuffers(1, this->GetObjectVBOPointer());
		glGenBuffers(1, this->GetObjectEBOPointer());
		

		//first, bind the VAO (de-reference the returned pointer!):
		glBindVertexArray(*this->GetObjectVAOPointer());

		//now, bind the VBO (de-reference the returned pointer!):
		glBindBuffer(GL_ARRAY_BUFFER, *this->GetObjectVBOPointer());
		//send the vertex data to the vertex buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (SURFACE_QUALITY * SURFACE_QUALITY), this->displayPoints, GL_STATIC_DRAW);

		//now, bind the EBO (de-reference the returned pointer!):
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this->GetObjectEBOPointer());
		//send the index data to the EBO:
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * this->NumIndices(), this->indices, GL_STATIC_DRAW);

		//next, we'll configure the vertex attribute pointer(s):
		//position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(GL_FLOAT)));
		//and enable it:
		glEnableVertexAttribArray(0);
		//DONT UNBIND THE EBO-- IT BREAKS LITERALLY EVERYTHING
		//unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//also unbind the VAO
		glBindVertexArray(0);
	};

	float displayPoints[3 * SURFACE_QUALITY * SURFACE_QUALITY];
	unsigned int indices[6 * SURFACE_QUALITY * SURFACE_QUALITY];
	unsigned int NumIndices() { return (6 * (SURFACE_QUALITY-1) * (SURFACE_QUALITY-1)); };

	virtual void CalculateDisplayPoints() = 0;

	double uMin = 0;
	double uMax = 0;

	double vMin = 0;
	double vMax = 0;

	std::string surfaceType;

	virtual glm::vec3 GetValueAtParams(double u, double v) = 0;

	void RenderObject(Shader* shader)
	{
		this->PassShaderData(shader);
		shader->Use();
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		shader->setVec4("flatColor", FRONT_FACE_COLOR);
		this->RenderAsTriangles(this->NumIndices());
		glCullFace(GL_BACK);
		shader->setVec4("flatColor", BACK_FACE_COLOR);
		this->RenderAsTriangles(this->NumIndices());
		glDisable(GL_CULL_FACE);
		
	}

	void PassShaderData(Shader* shader)
	{
		shader->Use();
		//then update the shader
		shader->setMat4("model", this->GetModelMatrix());

		//update the flat color and alpha
		float r, g, b, a;
		r = (float)this->GetFlatColor().r;
		g = (float)this->GetFlatColor().g;
		b = (float)this->GetFlatColor().b;
		a = (float)this->GetFlatColor().a;
		//scale them to be between 0 and 1 (divide by 255)
		r /= 255.0f;
		g /= 255.0f;
		b /= 255.0f;
		a /= 255.0f;
		//now, make  the color a vec4
		glm::vec4 flatColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		//update the flat color uniform and alpha uniform
		shader->setFloat("alpha", SURFACE_ALPHA);
	};

};

//class Revolve : public Surface
//{
//public:
//	Revolve() { this->surfaceType = "Revolve"; this->vMin = 0.0f; this->vMax = glm::two_pi<float>(); };
//	~Revolve() { this->parentSketch->RemoveDependent(); };
//
//	SketchCurve* profileCurve = nullptr;
//	SketchLine* revolveAxis = nullptr;
//	Sketch* parentSketch = nullptr;
//
//	float revolveAngle = 0.0f;
//
//
//	void SetProfileCurve(SketchCurve* curve)
//	{
//		if (this->parentSketch == nullptr)
//		{
//			return;
//		}
//
//		this->uMin = this->profileCurve->uMin;
//		this->uMax = this->profileCurve->uMax;
//
//		this->CalculateDisplayPoints();
//	};
//
//	void SetAxis(SketchLine* axis)
//	{
//		if (this->parentSketch == nullptr)
//		{
//			return;
//		}
//
//		this->revolveAxis = axis;
//
//		this->CalculateDisplayPoints();
//	}
//
//	void SetAngle(float angle)
//	{
//		this->revolveAngle = angle;
//		this->vMax = glm::radians(this->revolveAngle);
//		this->CalculateDisplayPoints();
//	};
//
//	void Update() {};
//
//	void CalculateDisplayPoints()
//	{
//		float pux[SURFACE_QUALITY * SURFACE_QUALITY];
//		float puy[SURFACE_QUALITY * SURFACE_QUALITY];
//		float puz[SURFACE_QUALITY * SURFACE_QUALITY];
//
//		double deltaV = (this->vMax - this->vMin);
//		double deltaU = ((double)this->uMax - this->uMin);
//
//		double dV = (deltaV) / ((double)SURFACE_QUALITY - 1);
//		double dU = (deltaU) / ((double)SURFACE_QUALITY - 1);
//		
//		for (int vIdx = 0; vIdx < SURFACE_QUALITY; vIdx++)
//		{
//			double vVal = dV * ((double)vIdx);
//			
//			for (int uIdx = 0; uIdx < SURFACE_QUALITY; uIdx++)
//			{
//
//				double uVal = dU * ((double)uIdx);
//
//				glm::vec3 surfacePoint = this->GetValueAtParams(uVal, vVal);
//
//				pux[uIdx + (vIdx * SURFACE_QUALITY)] = surfacePoint.x;
//				puy[uIdx + (vIdx * SURFACE_QUALITY)] = surfacePoint.y;
//				puz[uIdx + (vIdx * SURFACE_QUALITY)] = surfacePoint.z;
//			}
//		}
//
//		
//
//		for (int pIdx = 0; pIdx < SURFACE_QUALITY * SURFACE_QUALITY; pIdx++)
//		{
//			this->displayPoints[(3 * pIdx) + 0] = pux[pIdx];
//			this->displayPoints[(3 * pIdx) + 1] = puy[pIdx];
//			this->displayPoints[(3 * pIdx) + 2] = puz[pIdx];
//		}
//
//		this->SetUpBuffers();
//	};
//
//	glm::vec3 GetValueAtParams(double u, double v)
//	{
//		if (this->profileCurve == nullptr || this->revolveAxis == nullptr)
//		{
//			return glm::vec3(0.0f, 0.0f, 0.0f);
//		}
//		if (this->profileCurve->GetValueAtParam(u) == this->revolveAxis->GetObjectPosition())
//		{
//			return glm::vec3(this->profileCurve->GetValueAtParam(u));
//		}
//
//		//get translate matrix
//		//glm::mat4 translate = glm::mat4(1.0f);
//		//translate = glm::translate(translate, this->profileCurve->GetValueAtParam(u) - this->revolveAxis->GetObjectPosition());
//		//glm::mat4 invTranslate = glm::inverse(translate);
//
//		//get the original u, rotate through axis by v, translated into axis coordinates
//		glm::vec3 uCurvePos = this->profileCurve->GetValueAtParam(u);
//		//uCurvePos = translate * glm::vec4(uCurvePos, 1.0f);
//
//		//make the rotation matrix
//		glm::mat4 rotation = glm::mat4(1.0f);
//		rotation = glm::rotate(rotation, (float)v, this->revolveAxis->GetAxisDirection());
//
//		//now rotate
//		glm::vec3 surfacePos = rotation * glm::vec4(uCurvePos, 1.0f);
//
//		//and translate out of axis coordinates
//		//surfacePos = invTranslate * glm::vec4(surfacePos, 1.0f);
//
//		return surfacePos;
//
//	};
//
//};

class Loft : public Surface
{
public:
	Loft() { this->surfaceType = "Loft"; };
	~Loft() {
		for (int idx= 0; idx < this->numControlCurves; idx++)
		{
			if (this->inputCurves[idx] != nullptr)
			{
				this->inputCurves[idx]->RemoveDependent();
			}
		}
	};

	int numControlCurves = -1;
	Curve3D* inputCurves[MAX_CTRL_PTS];


	void SetCurves(int numCurves, Curve3D* inputCurves[])
	{
		this->numControlCurves = numCurves;
		for (int idx = 0; idx < this->numControlCurves; idx++)
		{
			this->inputCurves[idx] = inputCurves[idx];
			this->inputCurves[idx]->AddDependent();
		}

		this->CalculateDisplayPoints();
	};

	void Update() { this->CalculateDisplayPoints(); };

	void CalculateDisplayPoints()
	{
		float pux[SURFACE_QUALITY * SURFACE_QUALITY];
		float puy[SURFACE_QUALITY * SURFACE_QUALITY];
		float puz[SURFACE_QUALITY * SURFACE_QUALITY];

		//surface params s,tE[0,1]x[0,1]
		//curves are u,vE[uMin, uMax]x[vMin, vMax]
		//s->u is s*(uMax - uMin)
		//u->s is u/(uMax - uMin)
		
		
		//t->v is t*(vMax - vMin)
		//v->t is v/(vMax - vMin)


		double deltaU = this->uMax - this->uMin;
		double deltaV = this->vMax - this->vMin;

		for (int tIdx = 0; tIdx < SURFACE_QUALITY; tIdx++)
		{
			double v = (double)tIdx * deltaV * (1.0f / (double)(SURFACE_QUALITY - 1));
			for (int sIdx = 0; sIdx < SURFACE_QUALITY; sIdx++)
			{
				double u = (double)sIdx * deltaU * (1.0f / (double)(SURFACE_QUALITY - 1));

				glm::vec3 surfacePoint = this->GetValueAtParams(u, v);

				pux[sIdx + (SURFACE_QUALITY * tIdx)] = surfacePoint.x;
				puy[sIdx + (SURFACE_QUALITY * tIdx)] = surfacePoint.y;
				puz[sIdx + (SURFACE_QUALITY * tIdx)] = surfacePoint.z;
			}
		}

		for (int pIdx = 0; pIdx < SURFACE_QUALITY * SURFACE_QUALITY; pIdx++)
		{
			this->displayPoints[(3 * pIdx) + 0] = pux[pIdx];
			this->displayPoints[(3 * pIdx) + 1] = puy[pIdx];
			this->displayPoints[(3 * pIdx) + 2] = puz[pIdx];
		}

		this->SetUpBuffers();
	};

	glm::vec3 GetValueAtParams(double u, double v)
	{

	};

	void Init() {};
};

class Ruled : public Surface
{
public:
	Ruled() { this->surfaceType = "Ruled"; this->vMin = 0.0f; this->vMax = 1.0f; };
	~Ruled() { this->curve1->RemoveDependent(); this->curve2->RemoveDependent(); };

	Curve3D* curve1 = nullptr;
	Curve3D* curve2 = nullptr;


	void SetCurves(Curve3D* curve1, Curve3D* curve2)
	{
		if ((this->curve1 != nullptr) && (curve1 != this->curve1))
		{
			this->curve1->RemoveDependent();
		}

		if ((this->curve2 != nullptr) && (curve2 != this->curve2))
		{
			this->curve2->RemoveDependent();
		}

		this->curve1 = curve1;
		this->curve1->AddDependent();
		this->curve2 = curve2;
		this->curve2->AddDependent();

		this->Init();

		this->CalculateDisplayPoints();
	};

	//void Update() {  };

	void CalculateDisplayPoints()
	{
		float pux[SURFACE_QUALITY * SURFACE_QUALITY];
		float puy[SURFACE_QUALITY * SURFACE_QUALITY];
		float puz[SURFACE_QUALITY * SURFACE_QUALITY];

		//surface params s,tE[0,1]x[0,1]
		//curves are u,vE[uMin, uMax]x[vMin, vMax]
		//s->u is s*(uMax - uMin)
		//u->s is u/(uMax - uMin)


		//t->v is t*(vMax - vMin)
		//v->t is v/(vMax - vMin)


		double deltaU = this->uMax - this->uMin;
		double deltaV = this->vMax - this->vMin;

		for (int tIdx = 0; tIdx < SURFACE_QUALITY; tIdx++)
		{
			double v = (double)tIdx * deltaV * (1.0f / (double)(SURFACE_QUALITY - 1));
			for (int sIdx = 0; sIdx < SURFACE_QUALITY; sIdx++)
			{
				double u = (double)sIdx * deltaU * (1.0f / (double)(SURFACE_QUALITY - 1));

				glm::vec3 surfacePoint = this->GetValueAtParams(u, v);

				pux[sIdx + (SURFACE_QUALITY * tIdx)] = surfacePoint.x;
				puy[sIdx + (SURFACE_QUALITY * tIdx)] = surfacePoint.y;
				puz[sIdx + (SURFACE_QUALITY * tIdx)] = surfacePoint.z;
			}
		}

		for (int pIdx = 0; pIdx < SURFACE_QUALITY * SURFACE_QUALITY; pIdx++)
		{
			this->displayPoints[(3 * pIdx) + 0] = pux[pIdx];
			this->displayPoints[(3 * pIdx) + 1] = puy[pIdx];
			this->displayPoints[(3 * pIdx) + 2] = puz[pIdx];
		}

		this->SetUpBuffers();
	};

	void Init()
	{
		this->uMin = this->curve1->uMin;
		this->uMax = this->curve1->uMax;

		this->vMin = this->curve2->uMin;
		this->vMax = this->curve2->uMax;
	}

	glm::vec3 GetValueAtParams(double u, double v)
	{
		glm::vec3 c1 = this->curve1->GetValueAtParam(u);
		glm::vec3 c2 = this->curve2->GetValueAtParam(u);

		if (c1 == c2)
		{
			return c1;
		}

		glm::vec3 surfacePoint = glm::vec3((1.0 - v) * c1.x, (1.0 - v) * c1.y, (1.0 - v) * c1.z) + glm::vec3(v * c2.x, v * c2.y, v * c2.z);

		return surfacePoint;

	};
};