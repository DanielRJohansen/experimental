#include "ImageMarbleDetector.h"



ImageMarbleDetector::ImageMarbleDetector() {}


void ImageMarbleDetector::showSignsOfLife() {
	std::cout << "Me three!" << std::endl;
}


bool ImageMarbleDetector::isPixelInImage(Mat* image, int row, int col) {
	return 0 <= row && row < image->rows && 0 <= col && col < image->cols;
}
double* ImageMarbleDetector::calculateAverages(int listOfRGBs[9][3]) {

	static double averages[3];

	for (int channel = 0; channel < 3; channel++) {
		int sum = 0;
		int divisor = 9;
		for (int i = 0; i < 9; i++) {
			sum += listOfRGBs[i][channel];
			if (listOfRGBs[i][0] == 0 && listOfRGBs[i][1] == 0 && listOfRGBs[i][2] == 0) {
				divisor--;
			}
		}
		double avg = 0;
		if (divisor != 0) {
			avg = (double)sum / divisor;
		}
		averages[channel] = avg;
	}
	return averages;
}
double ImageMarbleDetector::calculateAverageVariance(int listOfRGBs[9][3]) {

	double* means = calculateAverages(listOfRGBs);
	// Flatten liste af RGB'er så listerne med 0'er ikke er der længere
	// eller tjek om det er en liste med 0'er og så tæl iteratoren ned? det tror jeg også man kan gøre
	double variances[3];

	for (int i = 0; i < 3; ++i) {
		double sum = 0;
		int numValidPixels = 0;
		for (int j = 0; j < 9; ++j) {
			if (!(listOfRGBs[i][0] == 0 && listOfRGBs[i][1] == 0 && listOfRGBs[i][2] == 0)) {
				sum += (listOfRGBs[j][i] - means[i]) * (listOfRGBs[j][i] - means[i]);
				numValidPixels++;
			}
		}
		double variance = (numValidPixels > 1) ? sum / (numValidPixels - 1) : 100;
		variances[i] = variance;
	}

	double avgVariance = 0;
	for (int k = 0; k < 3; ++k) {
		avgVariance += variances[k];
	}
	avgVariance /= 3;

	return avgVariance;
}
int ImageMarbleDetector::indexOfSmallestInList(double list[], int size) {
	int index = 0;
	double smallestVal = list[index];
	for (int i = 0; i < size; ++i) {
		if (list[i] < smallestVal) {
			index = i;
			smallestVal = list[i];
		}
	}
	return index;
}
void ImageMarbleDetector::rotateAroundPixelAtPosition(Mat* image, int row, int col) {

	double maskVariances[9];
	double maskAverages[9][3] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0} };

	int rgbForPixelsInMask[9][3] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0} };

	for (int rowOffset = 0; rowOffset < 3; rowOffset++) {
		for (int colOffset = 0; colOffset < 3; colOffset++) {

			int index = 0;

			for (int yOffset = -2; yOffset <= 0; yOffset++) {
				for (int xOffset = -2; xOffset <= 0; xOffset++) {

					int pixelRow = row + yOffset + rowOffset;
					int pixelCol = col + xOffset + colOffset;

					//cout << "[" << pixelRow << ", " << pixelCol << "]";

					if (isPixelInImage(image, pixelRow, pixelCol)) {
						//cout << " Inside";
						// If the position of a point in a given mask is inside the image, the RGB values of the pixel at that point are saved
						for (int i = 0; i < 3; i++) {
							rgbForPixelsInMask[index][i] = (int)image->at<Vec3b>(pixelRow, pixelCol)[i];
						}

					}
					else {
						//cout << " Outside";
					}
					index++;
					//cout << endl;
				}
			}
			// After run-through of a single mask
			// Calculate dispersion for mask and save in array maskDispersions (furthermore calculate dispersion for all color channels and save the average of these dispersions
			double variance = calculateAverageVariance(rgbForPixelsInMask);
			maskVariances[3 * rowOffset + colOffset] = variance;
			// Calculate average of mask and save in array maskAverages
			double* averages = calculateAverages(rgbForPixelsInMask);
			for (int j = 0; j < 3; j++) {
				//cout << "avg " << *(averages + j) << endl;
				maskAverages[3 * rowOffset + colOffset][j] = *(averages + j);
			}
			//cout << endl;
		}
	}
	// After run-through of all masks
	// Find mask with lowest dispersion and return average of that mask
	//coutList(maskAverages);
	int indexOfMaskWithSmallestVariance = indexOfSmallestInList(maskVariances, 9);
	auto r = (uchar)maskAverages[indexOfMaskWithSmallestVariance][0];
	auto g = (uchar)maskAverages[indexOfMaskWithSmallestVariance][1];
	auto b = (uchar)maskAverages[indexOfMaskWithSmallestVariance][2];

	Vec3b filteredPixel = { b,g,r };
	//cout << "b " << image->at<Vec3b>(row,col) << endl;
	image->at<Vec3b>(row, col) = filteredPixel;
	//cout << "a " << image->at<Vec3b>(row,col) << endl;
	//cout << endl;
}
void ImageMarbleDetector::removeOutliers(Mat* img, int minblacks)
{
	for (int y = 0; y < img->rows; y++) 
	{
		for (int x = 0; x < img->cols; x++) 
		{
			pixelcolor = img->at<uchar>(y, x);
			if (pixelcolor != 0)	//No need to fill already black ones
			{
				for (int yoff = -1; yoff < 2; yoff++)
				{
					for (int xoff = -1; xoff < 2; xoff++)
					{
						xpos = x + xoff;
						ypos = y + yoff;
						if (xpos > 0 && ypos > 0 && xpos < img->cols && ypos < img->rows)
						{
							kernelcolor = pixelcolor = img->at<uchar>(ypos, xpos);
							if (kernelcolor == 0)
							{
								adjecentblacks++;
							}
						}
					}
				}
				
				if (adjecentblacks > minblacks)
				{
					img->at<uchar>(y, x) = 0;
				}
				adjecentblacks = 0;
			}
		}
	}
}

void ImageMarbleDetector::colorPixel(Mat* img, int x, int y, int color)
{
	img->at<uchar>(y, x) = color;
	return;
}



int ImageMarbleDetector::imAt(Mat* img, int x, int y)
{
	return img->at<uchar>(y, x);
}

void ImageMarbleDetector::removeShades(Mat* img)
{
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = imAt(img, x, y);
			if (pixelcolor > 0 && pixelcolor < 255)
				colorPixel(img, x, y, 0);
		}
	}
}

void ImageMarbleDetector::drawEdges(Mat* img)
{
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);		
			if (pixelcolor != 0)	//No need to fill already black ones
			{
				for (int yoff = -1; yoff < 2; yoff++)
				{
					for (int xoff = -1; xoff < 2; xoff++)
					{
						xpos = x + xoff;
						ypos = y + yoff;
						
						if (xpos != lastx and ypos != lasty)
						{
							if (xpos > 0 && ypos > 0 && xpos < img->cols && ypos < img->rows)
							{
								kernelcolor =  img->at<uchar>(ypos, xpos);
								if (kernelcolor == 0)
								{
									if (yoff == 0 && xoff == 0)
									{
									//Do nothing

									}
									else
									{
									img->at<uchar>(y, x) = 255;
									//points[p_index][0] = x;
									//points[p_index][1] = y;
									//p_index++;
									goto breakkernel;
									}

									adjecentblacks++;
								}
							}
						}

					}
				}
				//img->at<uchar>(y, x) = 0;		//In case no adjecent black was found, we are not at edge


			breakkernel: {}
				lastx = x; lasty = y;

			}
		}
	}
	//outlineShapes(img);
}
void ImageMarbleDetector::removeFloors(Mat* img)
{
	//cout << "Rows: " << img->rows << "   Cols: " << img->cols << endl;
	floorcolor = (img->at<uchar>(img->rows - 1, img->cols / 2) + img->at<uchar>(img->rows - 1, img->cols / 2 + 1) + img->at<uchar>(img->rows - 1, img->cols / 2 + 1)) / 3;
	int ballcolor = img->at<uchar>(img->rows / 2, img->cols / 2);
	//cout << "floorcolor: " << floorcolor << "   Ballcolor: " << ballcolor << endl;

	for (int y = img->rows - 1; y >= 0; y--)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (floorcolor - pixelcolor < 20)
			{
				img->at<uchar>(y, x) = 0;
			}

		}
	}
}

Mat ImageMarbleDetector::rotatingMaskFilter(Mat* image) {
	Mat filteredImage = image->clone();

	for (int i = 0; i < filteredImage.rows; i++) {
		for (int j = 0; j < filteredImage.cols; j++) {
			rotateAroundPixelAtPosition(&filteredImage, i, j);
		}
	}

	return filteredImage;
}


void ImageMarbleDetector::shineMarker(Mat* img)
{
	for (int y = img->rows-1; y > 0; y--)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (pixelcolor != 255 && pixelcolor != 0)	//No need to fill already black ones
			{
				for (int yoff = -1; yoff < 1; yoff++)		//2x3 kernel!!!!
				{
					for (int xoff = -1; xoff < 2; xoff++)
					{
						xpos = x + xoff;
						ypos = y + yoff;
						if (xpos > 0 && ypos > 0 && xpos < img->cols && ypos < img->rows)
						{
							kernelcolor = img->at<uchar>(ypos, xpos);
							//cout << kernelcolor-pixelcolor << endl;
							if (kernelcolor != 255)
							{
								bool shine = (kernelcolor - pixelcolor) > 10;
								if ((kernelcolor - pixelcolor) > 5)
								{
									img->at<uchar>(y, x) = 255;
									goto FINISHPIXEL;
								}
							}
							
						}
					}
				}
			}
		FINISHPIXEL:
			//
			int i;
		}
	}
}



int ImageMarbleDetector::muddify(Mat* img)
{
	int whitepixels = 0;
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (pixelcolor == 255)	//No need to fill already black ones
			{
				whitepixels++;
				for (int yoff = -1; yoff < 2; yoff++)
				{
					for (int xoff = -1; xoff < 2; xoff++)
					{
						xpos = x + xoff;
						ypos = y + yoff;
						if (xpos > 0 && ypos > 0 && xpos < img->cols && ypos < img->rows)
						{
							kernelcolor = img->at<uchar>(ypos, xpos);
							if (kernelcolor != 255)
							{
								img->at<uchar>(ypos, xpos) = (kernelcolor + pixelcolor) / 2;
							}
						}
					}
				}
			}
		}
	}
	return whitepixels;
}

float** ImageMarbleDetector::pointify(Mat* img, int whitepixels)
{
	float** points = NULL;
	points = new float* [whitepixels];
	int index = 0;
	
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (pixelcolor == 255)	//No need to fill already black ones
			{
				//next white pixel
				points[index] = new float[2];
				int msum = 0; int xcom = 0; int ycom = 0;
				for (int yoff = -1; yoff < 2; yoff++)
				{
					for (int xoff = -1; xoff < 2; xoff++)
					{
						xpos = x + xoff;
						ypos = y + yoff;
						if (xpos > 0 && ypos > 0 && xpos < img->cols && ypos < img->rows)
						{
							kernelcolor = img->at<uchar>(ypos, xpos);
							if (kernelcolor != 255)
							{
								msum += kernelcolor;
								xcom += kernelcolor * xpos;
								ycom += kernelcolor * ypos;
								img->at<uchar>(ypos, xpos) = (kernelcolor + pixelcolor) / 2;
							}
						}
					}
				}
				xcom = xcom / msum;
				ycom = ycom / msum;
				points[index][0] = xcom;
				points[index][1] = ycom;
			}
		}
	}
	return points;
}


bool ImageMarbleDetector::optimizedCIM(Mat* img)
{
	int threshold = 5;
	int xpos;
	int ypos;

	uchar edgecolor = 0;

	//Binarify
	Mat cImg;
	cImg = img->clone();
	cImg = rotatingMaskFilter(img);
	cvtColor(cImg, cImg, COLOR_BGR2GRAY);
	removeFloors(&cImg);
	removeOutliers(&cImg, 5);
	//drawEdges(&cImg);
	//removeShades(&cImg);
	//muddify(&cImg);
	shineMarker(&cImg);
	removeShades(&cImg);
	removeOutliers(&cImg, 5);
	removeOutliers(&cImg, 5);

	namedWindow("image", WINDOW_GUI_NORMAL);
	imshow("image", cImg);
	waitKey();

	return true;
}

float ImageMarbleDetector::circlesInImage(Mat* img, bool draw)
{
	Mat cImg;
	cImg = img->clone();
	cImg = rotatingMaskFilter(img);
	cvtColor(cImg, cImg, COLOR_BGR2GRAY);
	removeFloors(&cImg);
	removeOutliers(&cImg, 5);
	shineMarker(&cImg);
	removeShades(&cImg);
	//These two screws up the algo here!
	//drawEdges(&cImg);
	//removeShades(&cImg);
	//muddify(&cImg);

	
	// Apply Hough Transform directly from opencv
	float firstangle;
	int largesradius = 0;
	vector<Vec3f>  circles;
	HoughCircles(cImg, circles, HOUGH_GRADIENT, 4.5, cImg.rows / 0.5, 50, 100, 5, 200);
	for (size_t i = 0; i < circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		float xcenter = circles[i][0];
		float width = cImg.cols;
		float angle = (xcenter / width) * 40 - 20;
		if (draw)
		{
			cout << "Circle at " << angle << " degrees" << endl;
			circle(cImg, center, radius, Scalar(255, 255, 255), 2, 8, 0);
			if (radius > largesradius)
			{
				firstangle = angle;
				largesradius = radius;
			}
		}
		
	}
	if (draw)
	{
		namedWindow("image", WINDOW_GUI_NORMAL);
		imshow("image", cImg);
		waitKey();
	}
	

	if (circles.size() == 0)
	{
		return -999;
	}
	else
		return firstangle;
}

ImageMarbleDetector::~ImageMarbleDetector() {}
