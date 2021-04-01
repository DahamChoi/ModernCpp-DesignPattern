#pragma once

#include "VisitorBase.h"
#include "ExpressionVisitor.h"

class Expression
{
public:
	virtual ~Expression() = default;
	virtual void accept(VisitorBase* obj)
	{
		using EV = ExpressionVisitor<Expression>;
		if (auto ev = dynamic_cast<EV*>(obj))
			ev->visit(this);
	}
};