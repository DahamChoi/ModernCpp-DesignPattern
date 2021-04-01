#pragma once

#include "Expression.h"

class AdditionExpression : public Expression
{
public:
	Expression* left, * right;

	AdditionExpression(Expression* const left, Expression* const right)
		: left{ left }, right{ right } {}

	~AdditionExpression()
	{
		delete left;
		delete right;
	}

	virtual void accept(VisitorBase* obj)
	{
		using EV = ExpressionVisitor<AdditionExpression>;
		if (auto ev = dynamic_cast<EV*>(obj))
			ev->visit(this);
	}
};