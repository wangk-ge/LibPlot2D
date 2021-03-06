/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  expressionTree.h
// Date:  5/6/2011
// Auth:  K. Loux
// Desc:  Handles user-specified mathematical operations on datasets.

#ifndef EXPRESSION_TREE_H_
#define EXPRESSION_TREE_H_

// Standard C++ headers
#include <queue>
#include <stack>
#include <string>
#include <memory>

// Local headers
#include "lp2d/utilities/managedList.h"
#include "lp2d/utilities/dataset2D.h"

// wxWidgets headers
#include <wx/wx.h>

namespace LibPlot2D
{

/// Class for processing user-specified mathematical operations.  Uses a
/// shunting yard algorithm to build and evaluate expression trees from
/// user-specified strings.
class ExpressionTree
{
public:
	/// Constructor.
	///
	/// \param list Pointer to a list of Dataset2D objects that may be
	///             referenced in the expression.
	explicit ExpressionTree(const ManagedList<const Dataset2D>* list = nullptr);

	/// Solves the specified expression.
	///
	/// \param expression       Expression to evaluate.
	/// \param solvedData [out] Data set containing the results of the solved
	///                         expression.
	/// \param xAxisFactor      Factor to multiply against x-axis data in order
	///                         to transform data to "as represented in file"
	///                         units.  Typically 1, but varied depending on
	///                         the originating DataFile.
	///
	/// \returns A description of any parsing/evaluation errors, or an empty
	///          string for success.
	wxString Solve(wxString expression, Dataset2D &solvedData,
		const double &xAxisFactor);

	/// Solves the specified expression by simplifying and combining like
	/// terms.
	///
	/// \param expression             Expression to evaluate.
	/// \param solvedExpression [out] The result of the simplified expression.
	///
	/// \returns A description of any parsing/evaluation errors, or an empty
	///          string for success.
	std::string Solve(std::string expression, std::string &solvedExpression);

	/// Breaks the specified expression string into separate terms.
	///
	/// \param s Expression string.
	///
	/// \returns A list of separate terms.
	static wxArrayString BreakApartTerms(const wxString &s);

	/// Processes each term to extract the value of the coefficient and the
	/// power to which the variable is raised.
	///
	/// \param terms List of terms to process.
	///
	/// \returns Values that describe each term.
	static std::vector<std::pair<int, double>> FindPowersAndCoefficients(
		const wxArrayString &terms);

private:
	static const unsigned int mPrintfPrecision;
	const ManagedList<const Dataset2D> *mList;

	double mXAxisFactor;

	std::queue<wxString> mOutputQueue;

	wxString ParseExpression(const wxString &expression);
	wxString ParseNext(const wxString &expression, bool &lastWasOperator,
		unsigned int &advance, std::stack<wxString> &operatorStack);
	wxString EvaluateExpression(Dataset2D &results);
	std::string EvaluateExpression(std::string &results);

	void ProcessOperator(std::stack<wxString> &operatorStack, const wxString &s);
	void ProcessCloseParenthese(std::stack<wxString> &operatorStack);

	Dataset2D GetSetFromList(const unsigned int &i) const;

	static bool NextIsNumber(const wxString &s, unsigned int *stop = nullptr, const bool &lastWasOperator = true);
	static bool NextIsDataset(const wxString &s, unsigned int *stop = nullptr, const bool &lastWasOperator = true);
	static bool NextIsFunction(const wxString &s, unsigned int *stop = nullptr);
	static bool NextIsOperator(const wxString &s, unsigned int *stop = nullptr);
	static bool NextIsS(const wxString &s, unsigned int *stop = nullptr);

	static unsigned int FindEndOfNextTerm(const wxString &s, const unsigned int &start);
	static int GetTermPower(const wxString &s, unsigned int &start, unsigned int &end);

	bool IsLeftAssociative(const wxChar &c) const;
	bool OperatorShift(const wxString &stackString, const wxString &newString) const;

	void PopStackToQueue(std::stack<wxString> &stack);
	bool EmptyStackToQueue(std::stack<wxString> &stack);
	unsigned int GetPrecedence(const wxString &s) const;

	void PushToStack(const double &value, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack) const;
	void PushToStack(const Dataset2D &dataset, std::stack<Dataset2D> &setStack,
		std::stack<bool> &useDoubleStack) const;
	bool PopFromStack(std::stack<double> &doubleStack, std::stack<Dataset2D> &setStack,
		std::stack<bool> &useDoubleStack, double &value, Dataset2D &dataset) const;

	Dataset2D ApplyFunction(const wxString &function, const Dataset2D &set) const;
	double ApplyFunction(const wxString &function, const double &value) const;
	Dataset2D ApplyOperation(const wxString &operation, const Dataset2D &first, const Dataset2D &second) const;
	Dataset2D ApplyOperation(const wxString &operation, const Dataset2D &first, const double &second) const;
	Dataset2D ApplyOperation(const wxString &operation, const double &first, const Dataset2D &second) const;
	double ApplyOperation(const wxString &operation, const double &first, const double &second) const;

	bool FunctionRequiresDataset(const wxString &function) const;

	bool EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateFunction(const wxString &function, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateUnaryOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateNumber(const wxString &number, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateDataset(const wxString &dataset, std::stack<Dataset2D> &setStack,
		std::stack<bool> &useDoubleStack, wxString &errorString) const;

	void PushToStack(const wxString &s, std::stack<wxString> &stringStack,
		std::stack<bool> &useDoubleStack) const;
	bool PopFromStack(std::stack<double> &doubleStack, std::stack<wxString> &stringStack,
		std::stack<bool> &useDoubleStack, wxString& string, double &value) const;

	bool EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<wxString> &stringStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<wxString> &stringStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateUnaryOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<wxString> &stringStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;

	wxString ApplyOperation(const wxString &operation, const wxString &first, const wxString &second) const;
	wxString ApplyOperation(const wxString &operation, const wxString &first, const double &second) const;
	wxString ApplyOperation(const wxString &operation, const double &first, const wxString &second) const;

	bool SetOperatorValid(const wxString &operation, const bool &leftOperandIsDouble) const;
	bool ParenthesesBalanced(const wxString &expression) const;
	static bool BeginningMatchesNoCase(const wxString &s, const wxString &target, unsigned int *length = nullptr);

	wxString StringAdd(const wxString &first, const double &second) const;
	wxString StringAdd(const double &first, const wxString &second) const;

	wxString StringSubtract(const wxString &first, const double &second) const;
	wxString StringSubtract(const double &first, const wxString &second) const;

	wxString StringMultiply(const wxString &first, const double &second) const;
	wxString StringMultiply(const wxString &first, const wxString &second) const;
	wxString StringMultiply(const double &first, const wxString &second) const;

	wxString StringDivide(const double &first, const wxString &second) const;

	wxString StringPower(const double &first, const wxString &second) const;

	void AddToExpressionString(wxString &expression, const double &coefficient, const int &power) const;
};

}// namespace LibPlot2D

#endif// EXPRESSION_TREE_H_
