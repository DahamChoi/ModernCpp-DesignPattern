#include <iostream>

#include "AdditionExpression.h"
#include "DoubleExpression.h"

#include "ExpressionPrinter.h"
#include "ExpressionEvalutor.h"

int main()
{
	auto e = new AdditionExpression{
		new AdditionExpression{
			new DoubleExpression{ 2 },
			new DoubleExpression{ 3 },
		}, new DoubleExpression{ 5 }
	};

	ExpressionPrinter ep;
	ExpressionEvalutor ee;
	ep.visit(e);
	ee.visit(e);
	std::cout << ep.str() << "=" << ee.str() << std::endl;
}