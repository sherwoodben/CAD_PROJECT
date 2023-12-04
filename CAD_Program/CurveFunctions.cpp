#include "CurveFunctions.h"
#include <iostream>

double CurveFunctions::BernsteinPolynomial(double u, int i, int n)
{
	double binomialCoeff = CurveFunctions::BinomialCoefficient(n, i);

	double fac1;
	if (i == 0)
	{
		fac1 = 1.0f;
	}
	else
	{
		fac1 = pow(u, i);
	}

	double fac2;

	if ((n - i) == 0)
	{
		fac2 = 1.0f;
	}
	else
	{
		fac2 = pow((1.0f - u), (n - i));
	}

	return (binomialCoeff * fac1 * fac2);
}

double CurveFunctions::BinomialCoefficient(int n, int i)
{
	uint64_t num = CurveFunctions::factorial(n);
	uint64_t den = CurveFunctions::factorial(i) * CurveFunctions::factorial(n - i);

	return ((double)num / (double)den);
}

double CurveFunctions::BasisFunction(double u, int i, int k, int n, int knots[])
{
	if (knots == nullptr)
	{
		return 0.0f;
	}
	if (k == 1)
	{
		if ((knots[i] != knots[i + 1]) && (knots[i] <= u) && (u <= knots[i + 1]))
		{
			if (u == 0.0f || u == (double)((n + 1) - (k - 1)))
			{
				return 1.0f;
			}
			if (knots[i] == u )
			{
				return 0.5f;
			}
			return 1.0f;
		}
		return 0.0f;
	}

	double Nikm1 = CurveFunctions::BasisFunction(u, i, k - 1, n, knots);
	double Nip1km1 = CurveFunctions::BasisFunction(u, i + 1, k - 1, n, knots);
	int denom1 = knots[i + k - 1] - knots[i];
	int denom2 = knots[i + k] - knots[i + 1];

	double num1 = (u - (double)knots[i]) * Nikm1;
	double num2 = ((double)knots[i + k] - u) * Nip1km1;

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

uint64_t CurveFunctions::factorial(int n)
{
	if (n == 0)
	{
		return 1;
	}
	return (uint64_t)n * CurveFunctions::factorial(n - 1);
}
