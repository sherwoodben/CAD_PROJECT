#pragma once

//need vectors
#include <vector>

//include "Point.h" for 2D/3D Points
#include "Point.h"

//general NURB class
class NURB
{
public:
	NURB() {};
	~NURB() {};

	unsigned int numCtrlPoints = 0;
	std::vector<std::vector<double>> basisCoefficients;

	unsigned int GetPolynomialOrder() { return this->polynomialOrder; };
	void SetPolynomialOrder(unsigned int newValue) { this->polynomialOrder = newValue; };

	virtual void AddControlPointAtIndex() {};
	virtual void RemoveControlPointAtIndex() {};
	virtual void AddControlPointAtEnd() {};
	virtual void RemoveControlPointAtEnd() {};

	unsigned int* GetBoundsOfU() { unsigned int bounds[2] = { this->uLower, this->uUpper }; return bounds; };
	void SetBoundsOfU(unsigned int lower, unsigned int upper) { this->uLower = lower; this->uUpper = upper; };

	virtual void Recalculate() {};

	unsigned int CalculateKnotValue(unsigned int knotIdx);

	void ClearBasisFunctions() { this->basisCoefficients.clear(); };

private:

	unsigned int polynomialOrder = 0;

	unsigned int uLower = 0;
	unsigned int uUpper = 0;
};

//2D NURB Struct
class NURB2D : public NURB
{
public:
	NURB2D();
	~NURB2D();

	void AddControlPointAtIndex(Point2D newPoint, unsigned int idx)
	{
		this->ctrlPoints.insert(this->ctrlPoints.begin() + idx, newPoint);
		this->numCtrlPoints++;
	};

	void RemoveControlPointAtIndex(unsigned int idx)
	{
		this->ctrlPoints.erase(this->ctrlPoints.begin() + idx);
		this->numCtrlPoints--;
	};

	void AddControlPoinAtEnd(Point2D newPoint)
	{
		this->AddControlPointAtIndex(newPoint, this->numCtrlPoints);
	};

	void RemoveControlPointAtEnd()
	{
		this->RemoveControlPointAtIndex(this->numCtrlPoints);
	};

	void Recalculate();

private:
	std::vector<Point2D> ctrlPoints = {};
};

//3D NURB Struct
class NURB3D : public NURB
{
public:
	NURB3D();
	~NURB3D();

	void AddControlPointAtIndex(Point3D newPoint, unsigned int idx)
	{
		this->ctrlPoints.insert(this->ctrlPoints.begin() + idx, newPoint);
		this->numCtrlPoints++;
	};

	void RemoveControlPointAtIndex(unsigned int idx)
	{
		this->ctrlPoints.erase(this->ctrlPoints.begin() + idx);
		this->numCtrlPoints--;
	};

	void AddControlPoinAtEnd(Point3D newPoint)
	{
		this->AddControlPointAtIndex(newPoint, this->numCtrlPoints);
	};

	void RemoveControlPointAtEnd()
	{
		this->RemoveControlPointAtIndex(this->numCtrlPoints);
	};

private:
	std::vector<Point3D> ctrlPoints = {};
};