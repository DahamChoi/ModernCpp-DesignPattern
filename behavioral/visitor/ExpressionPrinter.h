#pragma once

#include "ExpressionVisitor.h"
#include "DoubleExpression.h"
#include "AdditionExpression.h"

#include <string>
#include <sstream>

class ExpressionPrinter : public VisitorBase,
	public ExpressionVisitor<DoubleExpression>,
	public ExpressionVisitor<AdditionExpression>
{
public:
	void visit(DoubleExpression* obj) override
	{
		oss << obj->value;
	}

	void visit(AdditionExpression* obj) override
	{
		oss << "(";
		obj->left->accept(this);
		oss << "+";
		obj->right->accept(this);
		oss << ")";
	}

	std::string str() const { return oss.str(); }

private:
	std::ostringstream oss;
};