#pragma once

#include "DoubleExpression.h"
#include "AdditionExpression.h"

#include <string>

class ExpressionEvalutor : public VisitorBase,
	public ExpressionVisitor<DoubleExpression>,
	public ExpressionVisitor<AdditionExpression>
{
public:
	void visit(DoubleExpression* obj) override
	{
		result = obj->value;
	}

	void visit(AdditionExpression* obj) override
	{
		obj->left->accept(this);
		auto temp = result;
		obj->right->accept(this);
		result += temp;
	}

	std::string str() const { return std::to_string(result); }

private:
	double result{ 0 };
};