#ifndef _LABELER_SOURCE_
#define _LABELER_SOURCE_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "labeler.h"

using namespace std; 
using namespace cv;
using namespace boost::filesystem;

string labeler::checkLabeled(string &dir) {
	labeledDir = dir;
	labeledDir.append("\\training_imgs");
	cout << "checking cropped images" << endl;
	if (!exists(labeledDir)){
		cout << "creating directory" << labeledDir << endl;
		create_directory(labeledDir);
	}
	//search through the cropped images
	path p1(labeledDir);
	string fileName;
	Mat temp;
	for (directory_iterator s(p1); s != directory_iterator(); ++s) {
		fileName = s->path().filename().string();
		if (fileName.find("pos") != string::npos) {
			pNum++;
		}
		else if (fileName.find("neg") != string::npos) {
			nNum++;
		}
	}
	cout << "There are " << pNum + nNum << " cropped images" << endl;
	cout << pNum << " positives" << endl;
	cout << nNum << " negatives" << endl;
	return labeledDir; 
}

void labeler::storeCropped(vector<Mat> &positives, vector<Mat> &negatives) {
	char posN[40];
	char negN[40];
	Mat temp;
	for (int i = 0; i < positives.size(); i++) {
		sprintf_s(posN, "pos%d.PNG", pNum++);
		imwrite(labeledDir + "/" + posN, positives.at(i));
	}
	positives.clear();
	for (int i = 0; i < negatives.size(); i++) {
		sprintf_s(negN, "neg%d.PNG", nNum++);
		imwrite(labeledDir + "/" + negN, negatives.at(i));
	}
	negatives.clear();
}

void labeler::clearCropped(vector<Mat> &positives, vector<Mat> &negatives) {
	int dec = -1;
	while (dec != 2) {
		cout << "Enter 0 to erase the last negative" << endl;
		cout << "Enter 1 to erase the last positive" << endl;
		cout << "Enter 2 to continue cropping" << endl;
		cin >> dec;
		if (dec == 0) {
			if (!negatives.empty()) {
				negatives.pop_back();
				cout << "Erasing last negative ROI\n" << endl;
			}
			else {
				cout << "There are no newly added negative ROIs\n" << endl;
			}
		}
		else if (dec == 1) {
			if (!positives.empty()) {
				positives.pop_back();
				cout << "Erasing last positive ROI\n" << endl;
			}
			else {
				cout << "There are no newly added positive ROIs\n" << endl;
			}
		}
		else {
			cout << "Input not recognized." << endl;
			cout << "Back to labeling." << endl;
			break;
		}
	}
}

#endif