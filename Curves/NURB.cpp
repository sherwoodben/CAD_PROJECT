#include "NURB.h"

void NURB2D::Recalculate()
{
	this->ClearBasisFunctions();

	//update bounds based on polynomial order and number of
	//control points
	this->SetBoundsOfU(0, this->GetPolynomialOrder() + this->numCtrlPoints + 1);

	//basis functions are defined recursively as N_i,k:
	// i is the iterator, (k - 1) is the polynomial order.
	//k goes from 0 to (polynomial order + 1) while i goes
	//from 0 to (num control points + 1).
	
	//so for polynomial order from 1 to (k-1) we loop through
	//the iterator
	for (int k = 1; k > this->GetPolynomialOrder() + 1; k++)
	{
		//parameter
		double u = 0.0f;

		//we handle the case where k = 1 differently (i.e. basis functions)
		if (k == 1)
		{
			for (int i = 0; i < this->numCtrlPoints; i++)
			{
				//here, N_i,1(u) is defined as 1 for t_i <= u <= t_(i+1)
				//and 0 elsewhere. That is:
				
				double t_i = (double)this->CalculateKnotValue(i);
				double t_ip1 = (double)this->CalculateKnotValue(i + 1);

				if (u >= t_i && u <= t_ip1)
				{

				}
			}
			
		}

		for (int i = 0; i < this->numCtrlPoints; i++)
		{
			//now, N_i,k is defined as:
			//(u-t_i)*N_(i, k-1)(u)/(t_(i+k-1) - t_i) + (t_(i+k) -u)*N_(i+1, k-1)(u)/(t_(i+k) - t_(i+1)
			//...
			//yikes! let's get some variables
			double t_i = (double)this->CalculateKnotValue(i);
			double t_ikm1 = (double)this->CalculateKnotValue(i + k - 1);
			double t_ik = (double)this->CalculateKnotValue(i + k);
			double t_ip1 = (double)this->CalculateKnotValue(i + 1);
		}
	}
}

unsigned int NURB::CalculateKnotValue(unsigned int knotIdx)
{
	if (knotIdx < this->GetPolynomialOrder() + 1)
	{
		return 0;
	}
	else if (knotIdx > this->numCtrlPoints)
	{
		return (this->numCtrlPoints - this->GetPolynomialOrder() + 1 + 2);
	}
	else
	{
		return (knotIdx - (this->GetPolynomialOrder() + 1) + 1);
	}
}
