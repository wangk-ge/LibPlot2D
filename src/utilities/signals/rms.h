/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  rms.h
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes root-mean-square of data.
// History:

#ifndef RMS_H_
#define RMS_H_

// Local forward declarations
class Dataset2D;

class RootMeanSquare
{
public:
	static Dataset2D ComputeTimeHistory(const Dataset2D &data);
};

#endif// RMS_H_