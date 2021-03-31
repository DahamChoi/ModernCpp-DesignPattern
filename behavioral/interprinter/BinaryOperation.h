#pragma once

#include "Element.h"

#include <memory>

class BinaryOperation : public Element
{
public:
	enum Type { addition, subtraction } type;
	std::shared_ptr<Element> lhs, rhs;

	int eval() const override
	{
		return (type == addition) ?
			lhs->eval() + rhs->eval() : lhs->eval() - rhs->eval();
	}
};