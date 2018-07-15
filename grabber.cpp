#ifndef _GRABBER_SOURCE_
#define _GRABBER_SOURCE_

#include "grabber.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/ml.hpp> 

using namespace std;
using namespace cv;
using namespace boost::filesystem;


grabImg::grabImg() {
	cout << "grabber constructor called" << endl; 
}

grabImg::~grabImg() {
	cout << "grabber deconstructor called" << endl; 
}

void grabImg::setDirectory(string &dir) {
	do {
		cout << "Enter image directory: " << endl;
		getline(cin, directory);

		if (exists(directory))
			cout << "Directory exists" << endl;
		else 
			cout << "Directory does not exists" << endl;

	}while(!exists(directory)); 
	path p(directory);
	start = directory_iterator(p);	
}


string grabImg::getDirectory() {
	return directory;
}

void grabImg::getImg(Mat &img, string &format) {
	bool found = false;
	while (start != directory_iterator() && !found) {
		name = start->path().filename().string();
		if (!isImage(format)) {
			++start;
			continue;
		}
		found = true;
	}
	if (found)
		img = imread(start->path().string());
	else
		img.release();
}

void grabImg::getNextImg(Mat &img, string &format) {
	++start;
	bool found = false;
	while (start != directory_iterator() && !found) {
		name = start->path().filename().string();
		if (!isImage(format)) {
			++start;
			continue;
		}
		found = true;
	}
	if (found)
		img = imread(start->path().string());
	else
		img.release();
}

bool grabImg::isImage(string &format) {
	if (name.find(format) != string::npos)
		return true;
	return false;
}

#endif
