#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <iomanip>
#include <string>
#include <strsafe.h>
#include <math.h>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/ml.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc.hpp>
#include "grabber.h"
#include "labeler.h"
#include "trainer.h"

using namespace std; 
using namespace cv; 
using namespace boost::filesystem; 

//global variables
Mat image;
Mat img;
//mouse call back variables
Point p1L, p2L;
Point p1R, p2R;
bool drawingP = false;
bool drawingN = false;
vector<Mat> positives;
vector<Mat> negatives;

//call back function for image labeling
void mouseValues(int event, int x, int y, int flags, void* param) {
	image = img.clone();
	switch (event) {
		case CV_EVENT_MOUSEMOVE:
		{
			if (drawingP) {
				image = image.clone();
				rectangle(image, p1L, cvPoint(x, y), Scalar(255, 0, 0), 1);
				imshow("image", image);
			}
			if (drawingN) {
				image = image.clone();
				rectangle(image, p1R, cvPoint(x, y), Scalar(0, 0, 255), 1);
				imshow("image", image);
			}
			break;
		}
		case CV_EVENT_LBUTTONDOWN:
		{
			drawingP = true;
			p1L.x = x;
			p1L.y = y;
			//draw dot where first left mouse click happens
			image = img.clone();
			circle(image, p1L, 2, Scalar(255, 0, 0), 4);
			imshow("image", image);
			break;
		}
		case CV_EVENT_LBUTTONUP:
		{
			drawingP = false;
			p2L.x = x;
			p2L.y = y;
			//draw blue rectangle on image when mouse left button is released
			image = img.clone();
			rectangle(image, p1L, p2L, Scalar(255, 0, 0), 1);
			if (abs(p1L.x - p2L.x) > 10 && abs(p1L.y - p2L.y) > 10) {
				imshow("image", image);
				Mat roi(img, Rect(p1L, p2L));
				imshow("roiL", roi);
				resize(roi, roi, Size(80, 80));
				positives.push_back(roi);
			}
			else {
				cout << "ROIs have to be bigger than that" << endl;
			}
			break;
		}
		case CV_EVENT_RBUTTONDOWN:
		{
			drawingN = true;
			p1R.x = x;
			p1R.y = y;
			//draw dot where first left mouse click happens
			image = img.clone();
			circle(image, p1R, 2, Scalar(0, 0, 255), 4);
			imshow("image", image);
			break;
		}
		case CV_EVENT_RBUTTONUP:
		{
			drawingN = false;
			p2R.x = x;
			p2R.y = y;
			//draw blue rectangle on image when mouse left button is released
			image = img.clone();
			rectangle(image, p1R, p2R, Scalar(0, 0, 255), 1);
			if (abs(p1R.x - p2R.x) > 10 && abs(p1R.y - p2R.y) > 10) {
				imshow("image", image);
				Mat roi(img, Rect(p1R, p2R));
				imshow("roiR", roi);
				resize(roi, roi, Size(80, 80));
				negatives.push_back(roi);
			}
			else {
				cout << "ROIs have to be bigger than that" << endl; 
			}
			break;
		}
	}
	//draw on image while the window is opended
	imshow("image", image);
}



int main(){
	grabImg* grabber = new grabImg;
	labeler* label = new labeler; 
	trainer* trn = new trainer; 
    string dir;
	string labeledDir; 
	string format;
	grabber->setDirectory(dir); 
	labeledDir = label->checkLabeled(dir); 
	cout << "What is the image format: " << endl; 
	cin >> format; 
	bool done = false;
	grabber->getImg(img, format);
	while (!done) {
		if (!empty(img)) {
			namedWindow("image");
			cvSetMouseCallback("image", mouseValues, &img);
		}
		else {
			done = true;
		}
		int k = waitKey(0);
		if (k == 32) {
			int decision = -1; 
			cout << "Menu: " << endl;
			cout << "Want to clear a recently stored image? enter 0 " << endl;
			cout << "Enter 1 to continue cropping images." << endl; 
			cout << "Enter 2 to begin SVM training." << endl; 
			cout << "Enter 3 to exit" << endl; 
			cin >> decision; 
			if (decision == 0) {
				label->clearCropped(positives, negatives);
			}
			else if (decision == 3) {
				break; 
			}
			else {
				trn->prepare(labeledDir); 
				trn->train(); 
				cout << "Trained" << endl; 
				//cout << "testing..." << endl; 
				//trn.test(img); 
			}
				
		}
		label->storeCropped(positives, negatives);
		grabber->getNextImg(img, format); 
		cout << "Select image and press space key to enter menu" << endl;
	}
	image.release();
	delete grabber; 
	cout << "done!!" << endl; 
	cin >> format; 
    return 0;
}; 
