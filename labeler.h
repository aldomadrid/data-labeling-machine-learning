#ifndef _LABELER_H_
#define _LABELER_H_

#include <string>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp> 
using namespace std; 
using namespace cv; 

class labeler {
public:
	labeler(): pNum(0), nNum(0) { cout << "labeler constructor called" << endl; }
	virtual ~labeler() { cout << "labeler destructor called" << endl; }
	string checkLabeled(string &dir);
	void storeCropped(vector<Mat> &positives, vector<Mat> &negatives);
	void clearCropped(vector<Mat> &positives, vector<Mat> &negatives);
private:
	string labeledDir;
	int pNum;
	int nNum;
};

#endif