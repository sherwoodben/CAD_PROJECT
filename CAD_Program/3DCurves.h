#pragma once
#include <vector>
#include <math.h>

#include "SceneObject.h"
#include "Point.h"
#include "CurveFunctions.h"

struct Point3D
{
	Point3D(float x=0.0f, float y=0.0f, float z=0.0f) { this->pos = glm::vec3(x, y, z); }
	glm::vec3 pos;
};

class Curve3D : public SceneObject
{
public:
	Curve3D()
	{
		for (int idx = 0; idx < 3 * CURVE_QUALITY; idx++)
		{
			this->displayPoints[idx] = 0.0f;
		}

		this->InitCurve();
	};

	~Curve3D() {};

	void InitCurve()
	{
		this->isDatumObject = false;

		this->objectType = "Curve";

		
		this->SetShaderType(ShaderType::CURVE);
		//this->CalculateDisplayPoints();

		glGenVertexArrays(1, this->GetObjectVAOPointer());
		glBindVertexArray(*this->GetObjectVAOPointer());
		glGenBuffers(1, this->GetObjectVBOPointer());
		glBindVertexArray(0);
	};

	float displayPoints[3 * CURVE_QUALITY];

	virtual void CalculateDisplayPoints() = 0;

	double uMin = 0;
	double uMax = 0;

	virtual glm::vec3 GetValueAtParam(double u) = 0;

	void RenderObject(Shader* shader)
	{
		this->PassShaderData(shader);
		shader->Use();
		glBindVertexArray(*this->GetObjectVAOPointer());
		glBindBuffer(GL_ARRAY_BUFFER, *this->GetObjectVBOPointer());
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * CURVE_QUALITY, this->displayPoints, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glLineWidth(3.0f);
		glDrawArrays(GL_LINE_STRIP, 0, CURVE_QUALITY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	};

	void PassShaderData(Shader* shader)
	{
		shader->Use();

		shader->setMat4("model", this->GetModelMatrix());

		glm::vec4 flatColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		shader->setVec4("flatColor", flatColor);
		shader->setFloat("alpha", 1.0f);
	}
};

struct Arc3D : public Curve3D
{
	Point3D p1;
	Point3D p2;
	Point3D arcCenter;

	glm::vec3 normalVector;

	float radius;

	Arc3D(Point3D p1, Point3D arcCenter, float thetaExtent, glm::vec3 normalVector, bool reversed = false)
	{
		this->normalVector = normalVector;

		this->p1 = p1;

		this->arcCenter = arcCenter;

		glm::vec3 oP1 = this->p1.pos - this->arcCenter.pos;

		this->radius = sqrt(oP1.x * oP1.x + oP1.y * oP1.y + oP1.z * oP1.z);

		glm::vec3 yAxis = glm::cross(glm::normalize(oP1), glm::normalize(normalVector));

		this->p2.pos.x = radius * (glm::cos(thetaExtent) * glm::normalize(oP1).x + glm::sin(thetaExtent) * yAxis.x) + this->arcCenter.pos.x;
		this->p2.pos.y = radius * (glm::cos(thetaExtent) * glm::normalize(oP1).y + glm::sin(thetaExtent) * yAxis.y) + this->arcCenter.pos.y;
		this->p2.pos.z = radius * (glm::cos(thetaExtent) * glm::normalize(oP1).z + glm::sin(thetaExtent) * yAxis.z) + this->arcCenter.pos.z;

		this->CalculateDisplayPoints();
	}

	void CalculateDisplayPoints()
	{
		glm::vec3 oP1 = this->p1.pos - this->arcCenter.pos;
		glm::vec3 oP2 = this->p2.pos - this->arcCenter.pos;

		glm::vec3 a = glm::normalize(oP1);
		glm::vec3 b = glm::normalize(oP2);

		float angleBetween = glm::acos(glm::dot(a, b));
		/*if (angleBetween != glm::pi<float>())
		{

		}*/
		float radius = sqrt(oP1.x * oP1.x + oP1.y * oP1.y + oP1.z * oP1.z);

		glm::vec3 zAxis = glm::normalize(glm::cross(b, a));

		glm::vec3 xAxis = a;
		glm::vec3 yAxis = glm::cross(zAxis, xAxis);

		this->displayPoints[0] = this->p1.pos.x;
		this->displayPoints[1] = this->p1.pos.y;
		this->displayPoints[2] = this->p1.pos.z;

		for (int idx = 1; idx < CURVE_QUALITY - 1; idx++)
		{
			float theta = -(float)idx * angleBetween / (float)(CURVE_QUALITY - 1);
			this->displayPoints[(3 * idx)] = radius*(glm::cos(theta)*xAxis.x + glm::sin(theta)*yAxis.x ) + this->arcCenter.pos.x;
			this->displayPoints[(3 * idx) + 1] = radius*(glm::cos(theta) * xAxis.y + glm::sin(theta) * yAxis.y) + this->arcCenter.pos.y;
			this->displayPoints[(3 * idx) + 2] = radius*(glm::cos(theta) * xAxis.z + glm::sin(theta) * yAxis.z) + this->arcCenter.pos.z;
		}

		this->displayPoints[(3 * CURVE_QUALITY) - 3] = this->p2.pos.x;
		this->displayPoints[(3 * CURVE_QUALITY) - 2] = this->p2.pos.y;
		this->displayPoints[(3 * CURVE_QUALITY) - 1] = this->p2.pos.z;
	};
};

struct BSpline3D : public Curve3D
{
	bool isValid = false;

	int numControlPoints = -1;
	int polynomialOrder = -1;
	Point3D controlPoints[MAX_CTRL_PTS];

	int* knots = nullptr;
	int numKnots() { return (this->n() + this->k() + 1); };

	BSpline3D() {};

	~BSpline3D()
	{
		if (this->knots != nullptr)
		{
			delete[] this->knots;
		}
	};

	void UpdateKnots()
	{
		if (this->knots != nullptr)
		{
			delete[] knots;
		}

		this->knots = new int[this->n() + this->k() + 1];

		for (int knotIdx = 0; knotIdx < this->n() + this->k() + 1; knotIdx++)
		{
			if ((this->n() + this->k() + 1 < 1) || this->knots == nullptr)
			{
				return;
			}
			if (knotIdx < this->k())
			{
				this->knots[knotIdx] = 0;
			}
			else if ((this->k() <= knotIdx) && (knotIdx <= this->n()))
			{
				this->knots[knotIdx] = knotIdx - this->k() + 1;
			}
			else if ((knotIdx > this->n()) && (knotIdx < this->n() + this->k() + 1))
			{
				this->knots[knotIdx] = this->n() - this->k() + 2;
			}
		}

		for (int i = 0; i < this->n() + this->k() + 1; i++)
		{
			std::cout << this->knots[i] << std::endl;
		}
	}

	void SetControlPoints(int numPoints, int polynomialOrder, Point3D* controlPoints)
	{
		this->numControlPoints = numPoints;
		this->polynomialOrder = polynomialOrder;

		this->uMin = 0;
		this->uMax = (this->n() + 1) - (this->k() - 1);

		if (this->n() < (this->k() - 1))
		{
			this->isValid = false;
			this->uMin = 0.0f;
			this->uMax = 0.0f;
			return;
		}

		this->uMin = 0.0f;
		this->uMax = (this->n() + 1) - (this->k() - 1);

		this->isValid = true;

		for (int i = 0; i < numPoints; i++)
		{
			this->controlPoints[i] = controlPoints[i];
		}
		for (int i = numPoints; i < MAX_CTRL_PTS; i++)
		{
			this->controlPoints[i] = Point3D();
		}

		this->CalculateDisplayPoints();
	}

	int n()
	{
		if (this->numControlPoints > 0)
		{
			return this->numControlPoints - 1;
		}

		return 0;
	};

	int k()
	{
		if (this->polynomialOrder)
		{
			return this->polynomialOrder + 1;
		}

		return 0;
	};

	glm::vec3 GetValueAtParam(double u)
	{
		float pux = 0.0f;
		float puy = 0.0f;
		float puz = 0.0f;

		for (int i = 0; i < this->n() + 1; i++)
		{
			float basisFunction = CurveFunctions::BasisFunction(u, i, this->k(), this->n(), this->knots);
			pux += this->controlPoints[i].pos.x * basisFunction;
			puy += this->controlPoints[i].pos.y * basisFunction;
			puz += this->controlPoints[i].pos.z * basisFunction;
		}

		return glm::vec3(pux, puy, puz);
	}

	void CalculateDisplayPoints()
	{
		this->UpdateKnots();

		double deltaU = this->uMax - this->uMin;
		double uStep = deltaU / (double)(CURVE_QUALITY - 1);

		for (int displayPointIdx = 0; displayPointIdx < CURVE_QUALITY; displayPointIdx++)
		{
			double u = ((double)displayPointIdx * uStep) + this->uMin;

			glm::vec3 pos = this->GetValueAtParam(u);

			this->displayPoints[(3 * displayPointIdx)] = pos.x;
			this->displayPoints[(3 * displayPointIdx) + 1] = pos.y;
			this->displayPoints[(3 * displayPointIdx) + 2] = pos.z;

		}
	};
};

struct Bezier3D : public Curve3D
{
	bool isValid = false;

	int numControlPoints = -1;
	int polynomialOrder = -1;
	Point3D controlPoints[MAX_CTRL_PTS];

	Bezier3D(){};

	~Bezier3D() {};

	void SetControlPoints(int numPoints, Point3D* controlPoints)
	{
		this->numControlPoints = numPoints;
		this->polynomialOrder = numPoints - 1;

		if (this->n() != (this->numControlPoints - 1))
		{
			this->isValid = false;
			this->uMin = 0.0f;
			this->uMax = 1.0f;
			return;
		}

		this->uMin = 0.0f;
		this->uMax = 1.0f;

		this->isValid = true;

		for (int i = 0; i < numPoints; i++)
		{
			this->controlPoints[i] = controlPoints[i];
		}
		for (int i = numPoints; i < MAX_CTRL_PTS; i++)
		{
			this->controlPoints[i] = Point3D();
		}

		this->CalculateDisplayPoints();
	}

	int n()
	{
		return this->polynomialOrder;
	};


	glm::vec3 GetValueAtParam(double u)
	{
		float pux = 0.0f;
		float puy = 0.0f;
		float puz = 0.0f;

		for (int i = 0; i < this->n() + 1; i++)
		{
			float bernsteinPolynomial = CurveFunctions::BernsteinPolynomial(u, i, this->n());
			pux += this->controlPoints[i].pos.x * bernsteinPolynomial;
			puy += this->controlPoints[i].pos.y * bernsteinPolynomial;
			puz += this->controlPoints[i].pos.z * bernsteinPolynomial;
		}

		return glm::vec3(pux, puy, puz);
	};

	void CalculateDisplayPoints()
	{
		double deltaU = this->uMax - this->uMin;
		double uStep = deltaU / (double)(CURVE_QUALITY - 1);

		for (int displayPointIdx = 0; displayPointIdx < CURVE_QUALITY; displayPointIdx++)
		{
			double u = ((double)displayPointIdx * uStep) + this->uMin;
			
			glm::vec3 pos = this->GetValueAtParam(u);

			this->displayPoints[(3 * displayPointIdx)] = pos.x;
			this->displayPoints[(3 * displayPointIdx) + 1] = pos.y;
			this->displayPoints[(3 * displayPointIdx) + 2] = pos.z;

		}
	};
};