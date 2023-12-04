#pragma once
#include <vector>
#include <math.h>

#include "SceneObject.h"
#include "Plane.h"
#include "CurveFunctions.h"

#define MAX_CTRL_PTS 16
#define CURVE_QUALITY 33

struct SketchObject
{
	SketchObject() { this->objectID = this->objCounter++; };
	~SketchObject() {};

	std::string displayName = "";

	virtual void Render(Shader* shader) = 0;
	virtual void CalculateDisplayPoints() = 0;

	virtual std::string GetName() = 0;

	std::string GetNameAndID() { return (this->GetName() + " " + std::to_string(this->objectID)); };

	void SetDisplayName(std::string newName)
	{
		this->displayName = newName;
	}

	void DeleteObjects()
	{

	}

	bool isSelected = false;

	bool tryDelete = false;
	bool canDelete()  { return this->numDependents < 1; };

	void AddDependent() { this->numDependents++; };
	void removeDependent() { this->numDependents--; };

	int numDependents = 0;

	unsigned int objectID = -1;

	static unsigned int objCounter;
};

struct SketchPoint : public SketchObject
{
	void MovePoint(float x, float y) { this->pos = glm::vec2(x, y); };
	glm::vec2 pos = glm::vec2(0.0f, 0.0f);

	SketchPoint(float x, float y) : pos(glm::vec2(x, y)) { this->CalculateDisplayPoints(); };
	SketchPoint() { this->CalculateDisplayPoints(); };

	std::string GetName()
	{
		if (this->displayName == "")
		{
			return "Point";
		}
		
		return this->displayName;
	};

	float displayPoints[2] = {0.0f, 0.0f};

	void Render(Shader* shader)
	{
		shader->Use();
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, this->displayPoints, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glPointSize(50.f);
		shader->setVec4("objectColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		glDrawArrays(GL_POINTS, 0, 1);

		glDeleteBuffers(1, &VBO);

	};

	void CalculateDisplayPoints()
	{
		this->displayPoints[0] = this->pos.x;
		this->displayPoints[1] = this->pos.y;
	}
};

struct SketchCurve : public SketchObject
{
	float displayPoints[2 * CURVE_QUALITY];

	SketchCurve()
	{
		for (int idx = 0; idx < 2*CURVE_QUALITY; idx++)
		{
			this->displayPoints[idx] = 0.0f;
		}
	};

	~SketchCurve() {};

	double uMax = 0.0f;
	double uMin = 0.0f;

	virtual std::string GetName() = 0;

	virtual void CalculateDisplayPoints() = 0;

	void Render(Shader* shader)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * CURVE_QUALITY, this->displayPoints, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glLineWidth(3.0f);
		shader->setVec4("objectColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		glDrawArrays(GL_LINE_STRIP, 0, CURVE_QUALITY);

		glDeleteBuffers(1, &VBO);

	};
	
};

struct SketchLine : public SketchObject
{
	SketchPoint p1;
	SketchPoint p2;

	float displayPoints[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	SketchLine(SketchPoint point1, SketchPoint point2, bool reversed = false)
	{
		if (reversed)
		{
			this->p1 = point2;
			this->p2 = point1;
		}
		else
		{
			this->p1 = point1;
			this->p2 = point2;
		}

		this->CalculateDisplayPoints();
	};

	~SketchLine()
	{
	};

	std::string GetName()
	{
		if (this->displayName == "")
		{
			return "Line";
		}

		return this->displayName;
	};

	void CalculateDisplayPoints()
	{
		this->displayPoints[0] = this->p1.pos.x;
		this->displayPoints[1] = this->p1.pos.y;
		this->displayPoints[2] = this->p2.pos.x;
		this->displayPoints[3] = this->p2.pos.y;
	}


	void Render(Shader* shader)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		float data[4] = { p1.pos.x, p1.pos.y, p2.pos.x, p2.pos.y };
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, data, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glLineWidth(3.0f);
		shader->setVec4("objectColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		glDrawArrays(GL_LINES, 0, 2);

		glDeleteBuffers(1, &VBO);
	}
};

struct SketchArc : public SketchCurve
{
	SketchPoint p1;
	SketchPoint p2;
	SketchPoint arcCenter;
	float arcRadius;

	SketchArc(SketchPoint p1, SketchPoint p2, SketchPoint arcCenter, bool reversed = false)
		: SketchCurve()
	{
		if (reversed)
		{
			this->p1 = p2;
			this->p2 = p1;
			this->arcCenter = arcCenter;
		}
		else
		{
			this->p1 = p1;
			this->p2 = p2;
			this->arcCenter = arcCenter;
		}

		glm::vec2 OP1 = this->p1.pos - this->arcCenter.pos;
		glm::vec2 OP2 = this->p2.pos - this->arcCenter.pos;

		float rad1 = sqrt(OP2.x * OP2.x + OP2.y * OP2.y);
		float rad2 = sqrt(OP2.x * OP2.x + OP2.y * OP2.y);


		if (rad1 != rad2)
		{
			this->tryDelete = true;
		}
		else
		{
			this->CalculateDisplayPoints();
		}
	};

	SketchArc(SketchPoint arcCenter, SketchPoint p1, float thetaExtent) : SketchCurve()
	{
		this->p1 = p1;
		this->arcCenter = arcCenter;

		glm::vec2 OP1 = p1.pos - arcCenter.pos;
		this->arcRadius = sqrt(OP1.x*OP1.x + OP1.y*OP1.y);
		glm::vec2 axis = glm::normalize(OP1);
		float angle = glm::acos(glm::dot(axis, glm::vec2(1.0f, 0.0f)));

		float p2x = this->arcRadius * glm::cos(angle - thetaExtent) + arcCenter.pos.x;
		float p2y = this->arcRadius * glm::sin(angle - thetaExtent) + arcCenter.pos.y;

		this->p2 = SketchPoint(p2x, p2y);

		this->CalculateDisplayPoints();
	}

	std::string GetName()
	{
		if (this->displayName == "")
		{
			return "Arc";
		}

		return this->displayName;
	};

	void CalculateDisplayPoints()
	{
		glm::vec2 oP1 = this->p1.pos - this->arcCenter.pos;
		glm::vec2 oP2 = this->p2.pos - this->arcCenter.pos;

		glm::vec2 a = glm::normalize(oP1);
		glm::vec2 b = glm::normalize(oP2);

		float angleA = glm::acos(glm::dot(a, glm::vec2(1.0f, 0.0f)));
		float angleB = glm::acos(glm::dot(b, glm::vec2(1.0f, 0.0f)));

		float angleBetween = glm::acos(glm::dot(a, b));

		float radius = sqrt(oP1.x * oP1.x + oP1.y * oP1.y);

		this->displayPoints[0] = this->p1.pos.x;
		this->displayPoints[1] = this->p1.pos.y;

		for (int idx = 1; idx < CURVE_QUALITY - 1; idx++)
		{
			this->displayPoints[(2 * idx)] = radius * glm::cos(angleA - (float)idx * angleBetween / (float)(CURVE_QUALITY - 1)) + this->arcCenter.pos.x;
			this->displayPoints[(2 * idx) + 1] = radius * glm::sin(angleA - (float)idx * angleBetween / (float)(CURVE_QUALITY - 1)) + this->arcCenter.pos.y;
		}
		this->displayPoints[(2 * CURVE_QUALITY) - 2] = this->p2.pos.x;
		this->displayPoints[(2 * CURVE_QUALITY) - 1] = this->p2.pos.y;
	};


};

struct SketchBSpline : public SketchCurve
{
	bool isValid = false;

	int numControlPoints = -1;
	int polynomialOrder = -1;
	SketchPoint controlPoints[MAX_CTRL_PTS];

	int* knots = nullptr;
	int numKnots() { return (this->n() + this->k() + 1); };

	SketchBSpline() : SketchCurve()
	{
	};

	~SketchBSpline()
	{
		if (this->knots != nullptr)
		{
			delete[] this->knots;
		}
	}

	std::string GetName()
	{
		if (this->displayName == "")
		{
			return "B-Spline";
		}

		return this->displayName;
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
			//std::cout << this->knots[i] << std::endl;
		}
	}

	void SetControlPoints(int numPoints, int polynomialOrder, SketchPoint* controlPoints)
	{
		this->numControlPoints = numPoints;
		this->polynomialOrder = polynomialOrder;

		if (this->n() < (this->k() - 1))
		{
			this->isValid = false;
			return;
		}

		this->isValid = true;

		for (int i = 0; i < numPoints; i++)
		{
			this->controlPoints[i] = controlPoints[i];
		}
		for (int i = numPoints; i < MAX_CTRL_PTS; i++)
		{
			this->controlPoints[i] = SketchPoint();
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

	void CalculateDisplayPoints()
	{
		this->UpdateKnots();

		double uLower = 0;
		double uUpper = (this->n() + 1) - (this->k() - 1);
		double deltaU = uUpper - uLower;
		double uStep = deltaU / (double)(CURVE_QUALITY - 1);

		for (int displayPointIdx = 0; displayPointIdx < CURVE_QUALITY; displayPointIdx++)
		{
			double u = ((double)displayPointIdx*uStep) + uLower;
			float pux = 0.0f;
			float puy = 0.0f;

			for (int i = 0; i < this->n()+1; i++)
			{
				pux += this->controlPoints[i].pos.x * CurveFunctions::BasisFunction(u, i, this->k(), this->n(), this->knots);
				puy += this->controlPoints[i].pos.y * CurveFunctions::BasisFunction(u, i, this->k(), this->n(), this->knots);
			}

			this->displayPoints[(2 * displayPointIdx)] = pux;
			this->displayPoints[(2 * displayPointIdx) + 1] = puy;

		}
	};
};

struct SketchBezier : public SketchCurve
{
	bool isValid = false;

	int numControlPoints = -1;
	int polynomialOrder = -1;
	SketchPoint controlPoints[MAX_CTRL_PTS];

	SketchBezier() : SketchCurve()
	{
		this->displayName = "Bezier Curve";
	};

	~SketchBezier()
	{
	
	}

	std::string GetName()
	{
		if (this->displayName == "")
		{
			return "Bezier Curve";
		}

		return this->displayName;
	};

	void SetControlPoints(int numPoints, SketchPoint* controlPoints)
	{
		this->numControlPoints = numPoints;
		this->polynomialOrder = numPoints - 1;

		if (this->n() != (this->numControlPoints - 1))
		{
			this->isValid = false;
			return;
		}

		this->isValid = true;

		for (int i = 0; i < numPoints; i++)
		{
			this->controlPoints[i] = controlPoints[i];
		}
		for (int i = numPoints; i < MAX_CTRL_PTS; i++)
		{
			this->controlPoints[i] = SketchPoint();
		}

		this->CalculateDisplayPoints();
	}

	int n()
	{
		return this->polynomialOrder;
	};

	void CalculateDisplayPoints()
	{
		for (int displayPointIdx = 0; displayPointIdx < CURVE_QUALITY; displayPointIdx++)
		{
			double u = ((double)displayPointIdx / (double)(CURVE_QUALITY - 1));
			//std::cout << u << std::endl;
			float pux = 0.0f;
			float puy = 0.0f;

			for (int i = 0; i < this->n() + 1; i++)
			{
				pux += this->controlPoints[i].pos.x * CurveFunctions::BernsteinPolynomial(u, i, this->n());
				puy += this->controlPoints[i].pos.y * CurveFunctions::BernsteinPolynomial(u, i, this->n());
			}

			//std::cout << "(x,y): " << pux << ", " << puy << std::endl;
			this->displayPoints[(2 * displayPointIdx)] = pux;
			this->displayPoints[(2 * displayPointIdx) + 1] = puy;

		}
	};
};