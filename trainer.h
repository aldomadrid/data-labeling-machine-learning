#ifndef _TRAINER_H_
#define _TRAINER_H_

#include <string>
#include <iostream>
#include <opencv2/opencv.hpp> 


using namespace std;
using namespace cv;

class trainer {
public:
	trainer() : pNum(0), nNum(0) { cout << "trainer constructor called." << endl; }
	virtual ~trainer() { cout << "trainer destructor called" << endl; }
	void prepare(string &dir);
	void train();
	void test(Mat &img);
private:
	Mat training_mat;
	Mat labels;
	CvSVM svm;
	int pNum; 
	int nNum; 
};

#endif