/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  integral.cpp
// Date:  5/3/2011
// Auth:  K. Loux
// Desc:  Computes discrete-time integral of data.

// Local headers
#include "lp2d/utilities/signals/integral.h"
#include "lp2d/utilities/dataset2D.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			DiscreteIntegral
// Function:		ComputeTimeHistory (static)
//
// Description:		Computes the discrete-time integral time history for the
//					given signal.  Assumes y contains data and x is time.
//
// Input Arguments:
//		_data	= const Dataset2D& referring to the data of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the requested time history
//
//=============================================================================
Dataset2D DiscreteIntegral::ComputeTimeHistory(const Dataset2D &data)
{
	Dataset2D integral(data);

	if (data.GetNumberOfPoints() < 2)
		return integral;

	integral.GetY()[0] = 0.0;

	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); ++i)
		integral.GetY()[i] = integral.GetY()[i - 1] +
			(data.GetX()[i] - data.GetX()[i - 1]) * 0.5
			* (data.GetY()[i] + data.GetY()[i - 1]);

	return integral;
}

}// namespace LibPlot2D
