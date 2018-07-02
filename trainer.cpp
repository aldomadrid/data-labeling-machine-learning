#ifndef _TRAINER_SOURCE_
#define _TRAINER_SOURCE_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/ml.hpp> 
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "trainer.h"

using namespace std;
using namespace cv;
using namespace boost::filesystem;

void trainer::prepare(string &dir) {
	if (!exists(dir)) {
		cout << "there is no folder named training_imgs" << endl; 
		exit(0); 
	}

	path p1(dir);
	string fileName;
	Mat temp;
	vector<Mat> pImg; 
	vector<Mat> nImg; 
	for (directory_iterator s(p1); s != directory_iterator(); ++s) {
		fileName = s->path().filename().string();
		if (fileName.find("pos") != string::npos) {
			temp = imread(s->path().string(), 0); 
			pImg.push_back(temp); 
			pNum++;
		}
		else if (fileName.find("neg") != string::npos) {
			temp = imread(s->path().string(), 0);
			nImg.push_back(temp);
			nNum++;
		}
	}

	Mat tMat(pNum + nNum, 80 * 80, CV_32FC1);
	int ii = 0;
	int i;
	for (i = 0; i < pImg.size(); i++) {
		ii = 0;
		temp = pImg.at(i);
		for (int j = 0; j < temp.rows; j++) {
			for (int k = 0; k < temp.cols; k++) {
				tMat.at<float>(i, ii++) = float(temp.at<uchar>(j, k));
			}
		}
	}
	for (int a = i; a < i + nImg.size(); a++) {
		ii = 0;
		temp = nImg.at(a - i);
		for (int j = 0; j < temp.rows; j++) {
			for (int k = 0; k < temp.cols; k++) {
				tMat.at<float>(i, ii++) = float(temp.at<uchar>(j, k));
			}
		}
	}
	tMat.convertTo(tMat, CV_32FC1);
	training_mat = tMat;
	
	
	Mat l(pNum + nNum, 1, CV_32FC1);
	int x;
	for (x = 0; x < pNum; x++) {
		l.at<float>(x, 0) = 1.0;
	}
	int y = x;
	for (y; y < x + nNum; y++) {
		l.at<float>(y, 0) = -1.0;
	}
	labels = l;
}

void trainer::train() {
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.gamma = 3;
	params.degree = 3;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
	svm.train(training_mat, labels, Mat(), Mat(), params);
	/*save the xml and load it*/
	svm.save("gate_classifier.xml");
	cout << "classifier saved" << endl;
}

void trainer::test(Mat &img) {
	Mat temp = img.clone();
	cvtColor(temp, temp, CV_RGB2GRAY);
	svm.load("gate_classifier.xml");
	cout << "classifier loaded" << endl;
	if (temp.rows <= 80 || temp.cols <= 80) {
		resize(temp, temp, Size(80, 80));
		temp = temp.reshape(0, 1);
		temp.convertTo(temp, CV_32FC1);
		float res = svm.predict(temp);
		if (res > 0)
			cout << endl << "Found\n";
		else
			cout << endl << "Not Found\n";
	}
	else {
		//implement sliding window algorithm
		int w = 100;
		int h = 100;
		int x = 0;
		int y = 0;

		while (y + h < temp.rows) {
			x = 0;
			while (w + x < temp.cols) {
				Rect roi(x, y, w, h);
				Mat cI = img(roi);
				//imshow("cropped", cI); 
				cout << "cropping" << endl;
				float res = 0;
				if (roi.x >= 0 && roi.y >= 0 && roi.width + roi.x < temp.cols && roi.height + roi.y < temp.rows) {
					//cI = testImg(roi); 
					cout << "resizing" << endl;
					resize(cI, cI, Size(80, 80));
					cI.convertTo(cI, CV_32FC1);
					imshow("testing image", cI);
					cI = cI.reshape(0, 1);
					cout << "predicting" << endl;
					res = svm.predict(cI);
				}
				else {
					break;
					cout << "size not met" << endl;
				}
				if (res > 0) {
					cout << "Found\n" << endl;
					rectangle(temp, Point(x, y), Point(x + w, y + h), Scalar(255, 255, 255), 1);
				}
				else {
					cout << endl << "Not Found\n";
				}
				x += 40;
			}
			y += 40;
		}
		imshow("Test Image", temp);
		waitKey(0);
	}

}

#endif