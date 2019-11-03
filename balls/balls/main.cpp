#include "ImageMarbleDetector.h"

int main()
{
	ImageMarbleDetector imd;

	for (int i = 4; i < 7; i++)
	{
		string path = string("../images/") + to_string(i) + string(".png");

		Mat img = imread(path, IMREAD_ANYCOLOR);


		imd.optimizedCIM(&img);




	}


	return 0;
}