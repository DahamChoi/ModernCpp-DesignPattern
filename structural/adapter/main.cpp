#include "Painter.h"
#include "VectorRectangle.h"
#include "LineToPointAdapter.h"

int main()
{
	Painter p;
	VectorRectangle rectangle(0, 0, 10, 10);

//	p.DrawPoints(rectangle.begin(), rectangle.end());	Error!

	while (true)
	{
		for (auto& line : rectangle)
		{
			LineToPointAdapter ltp{ line };
			p.DrawPoints(ltp.begin(), ltp.end());
		}
	}
}