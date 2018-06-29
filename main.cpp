#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <stdint.h>
#include <direct.h>
#include <iomanip>
#include <windows.h>
#include <WinUser.h>
#include <vector> 
#include <boost/filesystem.hpp>
#include <strsafe.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/ml.hpp> 

using namespace std;  
using namespace cv;
using namespace boost::filesystem;

//global variables
vector<Mat> positives; 
vector<Mat> negatives; 
Mat image; 
Mat tImg; 
Point p1L, p2L; 
Point p1R, p2R; 
bool drawingP = false;
bool drawingN = false; 


//function that is called when cvSetMouseCallBack is called
//parameters are set according to opencv documentation
void mouseValues(int event, int x, int y, int flags, void* param) {
	Mat img = image.clone();
	switch (event) {
		case CV_EVENT_MOUSEMOVE:
		{
			if (drawingP){
				img = img.clone(); 
				rectangle(img, p1L, cvPoint(x, y), Scalar(255, 0, 0), 1);
				imshow("image", img); 
			}
			if (drawingN) {
				img = img.clone();
				rectangle(img, p1R, cvPoint(x, y), Scalar(0, 0, 255), 1);
				imshow("image", img);
			}
			break;
		}
		case CV_EVENT_LBUTTONDOWN:	       
		{
			drawingP = true;
			p1L.x = x; 
			p1L.y = y; 
			//draw dot where first left mouse click happens
			Mat img = image.clone();
			circle(img, p1L, 2, Scalar(255, 0, 0), 4);
			imshow("image", img);
			break;
		}
		case CV_EVENT_LBUTTONUP:
		{
			drawingP = false;
			p2L.x = x; 
			p2L.y = y; 
			//draw blue rectangle on image when mouse left button is released
			img = image.clone();
			rectangle(img, p1L, p2L, Scalar(255, 0, 0), 1);
			imshow("image", img);
			Mat roi(image, Rect(p1L, p2L));
			imshow("roiL", roi);
			resize(roi, roi, Size(80, 80));
			positives.push_back(roi); 
			break;
		}
		case CV_EVENT_RBUTTONDOWN: 
		{
			drawingN = true; 
			p1R.x = x; 
			p1R.y = y; 
			//draw dot where first left mouse click happens
			img = image.clone();
			circle(img, p1R, 2, Scalar(0, 0, 255), 4);
			imshow("image", img);
			break; 
		}
		case CV_EVENT_RBUTTONUP:
		{
			drawingN = false;
			p2R.x = x;
			p2R.y = y;
			//draw blue rectangle on image when mouse left button is released
			img = image.clone();
			rectangle(img, p1R, p2R, Scalar(0, 0, 255), 1);
			imshow("image", img);
			Mat roi(image, Rect(p1R, p2R));
			imshow("roiR", roi);
			resize(roi, roi, Size(80, 80)); 
			negatives.push_back(roi); 
			
			break;
		}
	}
	//draw on image while the window is opended
	imshow("image", img);
}
	
int main() {
	int eraseROI = -1; 
	int pNum = 0;
	int nNum = 0;
	int iPos = 0;
	int iNeg = 0;
	char pName[40]; 
	char nName[40]; 
	string picDir;
	string cDir; 
	//string posDir; 
	//string negDir; 
	vector<string> v;
	vector<string> t;
	WIN32_FIND_DATA data;
	HANDLE hFind;
	//ask for the directory of images
	cout << "What is location of the pictures: " << endl;
	getline(cin, picDir);
	cDir = picDir; 
    //picDir.append("\\*"); 
	cDir.append("\\");
	cDir.append("training_imgs"); 
	if (!exists(cDir.c_str())) {
		create_directory(cDir.c_str());
	}

	//Get the name of all the images 
	string imgName;
	path p1(picDir.c_str());
	for (directory_iterator d(p1); d != directory_iterator(); ++d) {
		imgName = d->path().filename().string();
		v.push_back(imgName);
	}

	//Get the names of already cropped images 
	path p(cDir.c_str());
	directory_iterator start(p); 
	for (start; start != directory_iterator(); ++start) {
		imgName = start->path().filename().string(); 
		t.push_back(imgName); 
	}

	//Get the number of neg and pos cropped images
	for (int i = 0; i < t.size(); i++) {
		imgName = t.at(i);
		if (imgName.find("pos") != string::npos)
			iPos++;
		else if (imgName.find("neg") != string::npos)
			iNeg++;
	}

	cout << "There are " << t.size() << " end. " << endl; 
	cout << iPos << " positives" << endl;
	cout << iNeg << " negatives" << endl; 

	//look for the first file in the folder
	//get the name of each file
	


	string dot = ".PNG";
	cout << "What is the image format? (Example: .jpg)" << endl;
	getline(cin, dot); 
	string dir; 
	size_t found; 
	//look for those files that have a .PNG extension
	for (int i = 0; i < v.size(); i++) {
		cout << "\nFile names: " << v.at(i) << endl;
		dir = v.at(i); 
		//select images with .PNG extension
		if (dir.find(dot) != string::npos) {
			//picDir.erase(picDir.end()-1, picDir.end());
			picDir.append("\\"); 
			picDir.append(dir); 
			cout << "\n" << picDir << " is an image" << endl;
			//read image with opencv
			image = cv::imread(picDir); 
			namedWindow("image");
			//allow user to select roi
			cvSetMouseCallback("image", mouseValues, &image);
			int k = waitKey(0);
			if (k == 32) {
				while (eraseROI != 2) {
					cout << "Do you want to erase a positive ROI? (Enter 0) " << endl;
					cout << "Do you want to erase a negative ROI? (Enter 1) " << endl;
					cout << "Do you want to move on to the next image? (Enger 2)" << endl;
					cout << "Do you want to begin SVM training with gray images? (Enger 3)" << endl;
					cin >> eraseROI;

					if (eraseROI == 0) {
						if (!positives.empty()) {
							positives.pop_back();
							cout << "Erasing last positive ROI\n" << endl;
						}
						else {
							cout << "There are no newly added positive ROIs\n" << endl;
						}
					}
					else if (eraseROI == 1) {
						if (!negatives.empty()) {
							negatives.pop_back();
							cout << "Erasing last negative ROI\n" << endl;
						}
						else {
							cout << "There are no newly added negative ROIs\n" << endl;
						}
					}
					else if (eraseROI == 3) {
						/*get the name of all cropped images*/
						directory_iterator s(p);
						vector<string> pName;
						vector<string> nName; 
						vector<Mat> pImg; 
						vector<Mat> nImg; 
						Mat temp; 
						for (s; s != directory_iterator(); ++s) {
							imgName = s->path().filename().string();
							if (imgName.find("pos") != string::npos) {
								pName.push_back(imgName);
								temp = imread(s->path().string(), 0); 
								pImg.push_back(temp);
							}
							else if (imgName.find("neg") != string::npos) {
								nName.push_back(imgName);
								temp = imread(s->path().string(), 0);
								nImg.push_back(temp);
							}
						}

						if (pName.size() >= 10 && nName.size() >= 10) {
							cout << "Collecting data.." << endl; 
							int fNum = 0;
							int pixNum = 0; 
							int dataNum; 
				
							//just in case
							/*get the number of usable images*/
							if (pName.size() < 20 || nName.size() < 20)  {
								dataNum = 10; 
							}
							else if (pName.size() < 30 || nName.size() < 30) {
								dataNum = 20;
							}
							else if (pName.size() < 40 || nName.size() < 40) {
								dataNum = 30;
							}
							else if (pName.size() < 50 || nName.size() < 50) {
								dataNum = 40;
							}
							else{
								dataNum = 100;
								cout << "Only the first 100 images will be used" << endl; 
							}

							cout << "Preparing trainning Matrix" << endl; 
							Mat tMat(pImg.size() + pImg.size(), 80 * 80, CV_32FC1);

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

							cout << "Preparing labels " << endl; 
							int kPos = 0;
							int kNeg = 0;
							string name; 
							string pos = "pos";
							string neg = "neg";
							Mat labels(pName.size() + nName.size(), 1, CV_32FC1);
							int x; 
							for (x = 0; x < pName.size(); x++) {
								labels.at<float>(x, 0) = 1.0; 
							}
							int y = x; 
							for (y; y < x + nName.size(); y++) {
								labels.at<float>(y, 0) = -1.0; 
							}
							
							
							cout << "training the machine" << endl; 
							CvSVMParams params;
							params.svm_type = CvSVM::C_SVC;
							params.kernel_type = CvSVM::LINEAR;
							params.gamma = 3;
							params.degree = 3;
							params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
							CvSVM svm;
							svm.train(tMat, labels, Mat(), Mat(), params);

							/*save the xml and load it*/
							svm.save("gate_classifier.xml");
							svm.load("gate_classifier.xml");

							/*test the trained data*/
							string tDir; 
							string tName; 
							cout << "testing the classifier" << endl; 
							cout << "Enter the path to the image you want to use: " << endl; 
							cin >> tDir;  
							Mat testImg = imread(tDir, 0); 
							imshow("Test Image", testImg);
							resize(testImg, testImg, Size(80, 80));
							testImg = testImg.reshape(0, 1);
							testImg.convertTo(testImg, CV_32FC1);
							float res = svm.predict(testImg);
							if (res > 0)
								cout << endl << "Found";
							else
								cout << endl << "Not Found";
							waitKey(0);

						}
						else {
							cout << "There need to be at least ten new positive images and 10 new negative images" << endl;
							cout << "to train your SVM" << endl; 
						}
						
					}
				}
			}
			picDir.erase(picDir.end() - dir.size(), picDir.end());
			cout << "\n" << picDir << " is also an image" << endl;
			//write images to a folder
			for (int i = 0; i < positives.size(); i++) {
				sprintf_s(pName, "pos%d.PNG", pNum++); 
				imwrite(cDir + pName, positives.at(i)); 
			}
			positives.erase(positives.begin(), positives.end()); 
			for (int i = 0; i < negatives.size(); i++) {
				sprintf_s(nName, "neg%d.PNG", nNum++);
				imwrite(cDir + nName, negatives.at(i));
			}
			negatives.erase(negatives.begin(), negatives.end());
		}
	}
	waitKey(0); 
	cout << "\nEnd of files" << endl;                                            
	return 0; 
}

