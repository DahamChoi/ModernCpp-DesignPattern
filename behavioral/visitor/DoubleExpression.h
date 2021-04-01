#pragma once

#include "Expression.h"

class DoubleExpression : public Expression
{
public:
	double value;

	DoubleExpression(const double value)
		: value{ value } {}

	virtual void accept(VisitorBase* obj)
	{
		using EV = ExpressionVisitor<DoubleExpression>;
		if (auto ev = dynamic_cast<EV*>(obj))
			ev->visit(this);
	}
};