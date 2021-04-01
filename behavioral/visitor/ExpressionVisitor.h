#pragma once

template<typename Visitable>
class ExpressionVisitor
{
public:
	virtual void visit(Visitable* obj) = 0;
};