#pragma once
#include <math.h>
#include <stdint.h>

namespace CurveFunctions
{
	double BernsteinPolynomial(double u, int i, int n);
	double BinomialCoefficient(int n, int i);
	double BasisFunction(double u, int i, int k, int n, int knots[]);
	uint64_t factorial(int n);
};