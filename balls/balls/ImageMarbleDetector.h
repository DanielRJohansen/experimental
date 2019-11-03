#pragma once

#ifndef IMAGEMARBLEDETECTOR_H
#define IMAGEMARBLEDETECTOR_H

#include <iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class ImageMarbleDetector {
public:
	ImageMarbleDetector();
	void showSignsOfLife();
	float circlesInImage(Mat * img, bool draw);
	bool optimizedCIM(Mat* img);

	~ImageMarbleDetector();



protected:
	int whitepixels = 0;
	int floorcolor;
	int pixelcolor; int kernelcolor;
	int xpos; int ypos;
	int adjecentblacks = 0;
	int lastx; int lasty;


	bool isPixelInImage(Mat* image, int row, int col);
	double* calculateAverages(int listOfRGBs[9][3]);
	double calculateAverageVariance(int listOfRGBs[9][3]);
	int indexOfSmallestInList(double list[], int size);
	void rotateAroundPixelAtPosition(Mat* image, int row, int col);
	Mat rotatingMaskFilter(Mat * img);


	void removeFloors(Mat* img);
	void removeOutliers(Mat* img, int minblacks);
	void drawEdges(Mat* img);
	void colorPixel(Mat* img, int x, int y, int color);
	void removeShades(Mat* img);
	int imAt(Mat* img, int x, int y);


	//Experimental
	void shineMarker(Mat* img);
	int segmentCOM(Mat* img, int xsegments=6, int ysegments=6);

	int muddify(Mat* img);	//Returns # of white pixels
	float** pointify(Mat* img, int whitepixels);
};


#endif //IMAGEMARBLEDETECTOR_H