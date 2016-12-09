/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  rangeLimitsDialog.h
// Created:  5/12/2011
// Author:  K. Loux
// Description:  Dialog box for entering a minimum and maximum value for an axis.
// History:

#ifndef RANGE_LIMITS_DIALOG_H_
#define RANGE_LIMITS_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

namespace LibPlot2D
{

class RangeLimitsDialog : public wxDialog
{
public:
	// Constructor
	RangeLimitsDialog(wxWindow *parent, const double &min, const double &max);

	double GetMinimum() const;
	double GetMaximum() const;

private:
	wxTextCtrl *minBox;
	wxTextCtrl *maxBox;

	// Overload from wxDialog
	virtual void OnOKButton(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE();
};

}// namespace LibPlot2D

#endif// RANGE_LIMITS_DIALOG_H_