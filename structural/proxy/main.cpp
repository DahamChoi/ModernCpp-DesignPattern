#include "Bitmap.h"
#include "LazyBitmap.h"

int main()
{
	Bitmap b("file1.bmp");			// 여기서 로드된다!
	LazyBitmap lb("file2.bmp");
	b.draw();
	lb.draw();						// 여기서 로드된다!
}