#ifndef _GRABBER_H_
#define _GRABBER_H_

#include <string>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp> 
#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std; 
using namespace cv;

class grabImg {
public:
	grabImg(); 
	virtual ~grabImg();
	void setDirectory(string &dir);
	string getDirectory();
	void getImg(Mat &img, string &format);
	void getNextImg(Mat &img, string &format);
protected:
	string directory;
	string name;
	directory_iterator start;
	bool isImage(string &format);
};


#endif
