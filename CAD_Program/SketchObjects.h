#pragma once
#include <vector>
#include <math.h>

#include "SceneObject.h"
#include "Plane.h"

#define MAX_CTRL_PTS 16
#define CURVE_QUALITY 65

struct SketchPoint
{
	glm::vec2 pos = glm::vec2(0.0f, 0.0f);

	SketchPoint(float x, float y) : pos(glm::vec2(x, y)) {};
	SketchPoint() {};

	void Render(Shader* shader)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		float data[2] = { pos.x, pos.y };
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, data, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glPointSize(12.5f);
		shader->setVec4("objectColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		glDrawArrays(GL_POINTS, 0, 1);

		glDeleteBuffers(1, &VBO);

	};
};

struct SketchCurve
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

	virtual void CalculateDisplayPoints() = 0;

	virtual void Render(Shader* shader)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * CURVE_QUALITY, this->displayPoints, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glLineWidth(2.5f);
		shader->setVec4("objectColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		glDrawArrays(GL_LINE_STRIP, 0, CURVE_QUALITY);

		glDeleteBuffers(1, &VBO);

	};
	
};

struct SketchLine
{
	SketchPoint p1;
	SketchPoint p2;

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

	void CalculateDisplayPoints()
	{
		
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

		glLineWidth(2.5f);
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

		this->CalculateDisplayPoints();

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
	
	double CalculateBernsteinPolynomial(double curveParam, int i, int k)
	{
		if (k == 1)
		{

			if ((this->knots[i] != this->knots[i + 1]) && (this->knots[i] <= curveParam) && (curveParam <= this->knots[i + 1]))
			{
				if (curveParam == 0.0f || curveParam == (double)((this->n() + 1) - (this->k() - 1)))
				{
					return 1.0f;
				}
				if (this->knots[i] == curveParam || this->knots[i+1] == curveParam)
				{
					return 0.5f;
				}
				return 1.0f;
			}
			return 0.0f;
		}

		double Nikm1 = this->CalculateBernsteinPolynomial(curveParam, i, k - 1);
		double Nip1km1 = this->CalculateBernsteinPolynomial(curveParam, i + 1, k - 1);
		int denom1 = this->knots[i + k - 1] - this->knots[i];
		int denom2 = this->knots[i + k] - this->knots[i + 1];

		double num1 = (curveParam - (double)this->knots[i]) * Nikm1;
		double num2 = ((double)this->knots[i + k] - curveParam) * Nip1km1;

		double term1;
		if ((num1 == denom1) && (num1 == 0.0f))
		{
			term1 = 0.0f;
		}
		else
		{
			term1 = num1 / denom1;
		}

		double term2;
		if ((num2 == denom2) && (num2 == 0.0f))
		{
			term2 = 0.0f;
		}
		else
		{
			term2 = num2 / denom2;
		}
		return (term1 + term2);

	};

	void UpdateKnots()
	{
		if (this->knots != nullptr)
		{
			delete[] this->knots;
		}

		this->knots = new int[this->numKnots()];
		
		for (int knotIdx = 0; knotIdx < this->numKnots(); knotIdx++)
		{
			if (this->knots == nullptr)
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
			else if (knotIdx > this->n() && (knotIdx < this->numKnots()))
			{
				this->knots[knotIdx] = this->n() - this->k() + 2;
			}
		}
	}

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
				pux += this->controlPoints[i].pos.x * CalculateBernsteinPolynomial(u, i, this->k());
				puy += this->controlPoints[i].pos.y * CalculateBernsteinPolynomial(u, i, this->k());
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

	};

	~SketchBezier()
	{
	
	}

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

	double CalculateBernsteinPolynomial(double curveParam, int i, int n)
	{
		double binomialCoeff = this->BinomialCoefficient(n, i);
		
		double fac1;
		if (i == 0)
		{
			fac1 = 1.0f;
		}
		else
		{
			fac1 = pow(curveParam, i);
		}
		
		double fac2;
		
		if ((n - i) == 0)
		{
			fac2 = 1.0f;
		}
		else
		{
			fac2 = pow((1.0f - curveParam), (n - i));
		}
		
		return (binomialCoeff * fac1 * fac2);


	};

	double BinomialCoefficient(int n, int i)
	{
		uint64_t num = this->factorial(n);
		uint64_t den = this->factorial(i) * this->factorial(n - i);

		return (num / den);
	};

	uint64_t factorial(int n)
	{
		if (n == 0)
		{
			return 1;
		}
		return (uint64_t)n * this->factorial(n - 1);
	};


	void CalculateDisplayPoints()
	{
		for (int displayPointIdx = 0; displayPointIdx < CURVE_QUALITY; displayPointIdx++)
		{
			double u = ((double)displayPointIdx / (double)(CURVE_QUALITY - 1));
			std::cout << u << std::endl;
			float pux = 0.0f;
			float puy = 0.0f;

			for (int i = 0; i < this->n() + 1; i++)
			{
				pux += this->controlPoints[i].pos.x * CalculateBernsteinPolynomial(u, i, this->n());
				puy += this->controlPoints[i].pos.y * CalculateBernsteinPolynomial(u, i, this->n());
			}

			std::cout << "(x,y): " << pux << ", " << puy << std::endl;
			this->displayPoints[(2 * displayPointIdx)] = pux;
			this->displayPoints[(2 * displayPointIdx) + 1] = puy;

		}
	};
};