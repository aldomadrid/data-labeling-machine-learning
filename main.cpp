#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <stdint.h>
#include <iomanip>
#include <windows.h>
#include <WinUser.h>
#include <vector> 
#include <strsafe.h>
#include <opencv2/core/core.hpp>
//#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>


using namespace std;  
using namespace cv;

Mat image, tImg; 
Point point1, point2; 
bool drawing = false; 

void mouseValues(int event, int x, int y, int flags, void* param) {
	switch (event) {
		case CV_EVENT_MOUSEMOVE:
		{
			if (drawing){
				//rectangle(image, point1, cvPoint(x, y), Scalar(255, 0, 0), 1);
			}
			break;
		}
		case CV_EVENT_LBUTTONDOWN:
		{
			drawing = true;
			point1.x = x; 
			point1.y = y; 
			circle(image, point1, 2, Scalar(255, 0, 0), 4);
			break;
		}
		case CV_EVENT_LBUTTONUP:
		{
			drawing = false;
			point2.x = x; 
			point2.y = y; 
			rectangle(image, point1, point2, Scalar(255, 0, 0), 1);
			Mat roi(image, Rect(point1, point2)); 
			imshow("roi", roi); 
			break;
		}
	}
	imshow("image", image);
}
	
int main() {
	string picDir;
	vector<string> v; 
	cout << "What is location of the picture: " << endl;
	getline(cin, picDir);
	picDir.append("\\*"); 
	WIN32_FIND_DATA data; 
	HANDLE hFind; 
	if ((hFind = FindFirstFile(picDir.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v.push_back(data.cFileName); 
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind); 
	}
	string dot = ".PNG";
	string dir; 
	size_t found; 
	
	for (int i = 0; i < v.size(); i++) {
		cout << "\nFile names: " << v.at(i) << endl;
		dir = v.at(i); 
		if (dir.find(dot) != string::npos) {
			picDir.erase(picDir.end()-2, picDir.end());
			picDir.append("\\"); 
			picDir.append(dir); 
			cout << "\n" << picDir << " is an image" << endl;
			image = cv::imread(picDir);
			namedWindow("image"); 
			cvSetMouseCallback("image", mouseValues, &image);
			waitKey(0); 
			picDir.erase(picDir.end() - dir.size() + 1, picDir.end());
			cout << "\n" << picDir << " directory" << endl;
		}
	}
	cout << "\nEnd of files" << endl; 
	cin >> picDir; 
	return 0; 
}

