#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <stdint.h>
#include <direct.h>
#include <iomanip>
#include <windows.h>
#include <WinUser.h>
#include <vector> 
#include <strsafe.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/ml.hpp> 

using namespace std;  
using namespace cv;

//global variables
vector<Mat> positives; 
vector<Mat> negatives; 
Mat image, tImg; 
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
			resize(roi, roi, Size(80, 80));
			positives.push_back(roi); 
			imshow("roiL", roi); 
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
			resize(roi, roi, Size(80, 80)); 
			negatives.push_back(roi); 
			imshow("roiR", roi);
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
	char pName[40]; 
	char nName[40]; 
	string picDir;
	string cDir; 
	//string posDir; 
	//string negDir; 
	vector<string> v; 
	//ask for the directory of images
	cout << "What is location of the pictures: " << endl;
	getline(cin, picDir);
	cDir = picDir; 
	//negDir = picDir; 
	picDir.append("\\*"); 
	cDir.append("\\");
	//negDir.append("\\");
	cDir.append("trainning_imgs"); 
	//negDir.append("negatives"); 
	CreateDirectory(cDir.c_str(), NULL); 
	//CreateDirectory(negDir.c_str(), NULL);
	
	WIN32_FIND_DATA data;
	HANDLE hFind;
	//look for the first file in the folder
	//get the name of each file
	if ((hFind = FindFirstFile(picDir.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v.push_back(data.cFileName); 
		} while (FindNextFile(hFind, &data) != 0);   //continue until there are no more files
		FindClose(hFind); 
	}
	string dot = ".PNG";
	cout << "What is the image format? (Example: .jpg)" << endl;
	cin >> dot; 
	string dir; 
	size_t found; 
	//look for those files that have a .PNG extension
	for (int i = 0; i < v.size(); i++) {
		cout << "\nFile names: " << v.at(i) << endl;
		dir = v.at(i); 
		//select images with .PNG extension
		if (dir.find(dot) != string::npos) {
			picDir.erase(picDir.end()-2, picDir.end());
			picDir.append("\\"); 
			picDir.append(dir); 
			cout << "\n" << picDir << " is an image" << endl;
			//read image with opencv
			image = cv::imread(picDir);
			tImg = image; 
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
						
						if (pNum >= 10 && nNum >= 10) {
							cout << "Collecting data.." << endl; 
							const int nLabels = pNum + nNum;
							
							int fNum = 0;
							int pixNum = 0; 
							int dataNum = pNum / 10;
							if (nNum > pNum) {
								dataNum = pNum / 10;  
							}
							else{
								dataNum = nNum / 10;
							}

							Mat training_mat(dataNum*2, 80 * 80, CV_32FC1);
							cout << dataNum << " number of images to train" << endl; 
							string name; 
							string pos = "pos";
							string neg = "neg";
							float labels10[10];
							float labels20[20]; 
							float labels30[30]; 
							float labels40[40]; 
							float labels100[100]; 
							//vector<float> labels; 
							if ((hFind = FindFirstFile(picDir.c_str(), &data)) != INVALID_HANDLE_VALUE) {
								do {
									name = data.cFileName; 
									tImg = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
									//Mat hsv; 
									//cvtColor(tImg, hsv, COLOR_BGR2HSV);
									//do some filtering here 
									//add the image to the training mat
									for (int i = 0; i < tImg.rows; i++) {
										for (int j = 0; j < tImg.cols; j++) {
											training_mat.at<float>(fNum, pixNum++) = tImg.at<uchar>(i, j); 
										}
									}
									switch (dataNum) {
										//fill a labels mat with 1 for every positive and -1 for every negative image
									case 1: {
												if (name.find(pos) != string::npos) labels10[fNum] = 1;
												else								labels10[fNum] = -1;
									}
									case 2: {
												if (name.find(pos) != string::npos) labels20[fNum] = 1;
												else								labels20[fNum] = -1;
									}
									case 3: {
												if (name.find(pos) != string::npos) labels30[fNum] = 1;
												else								labels30[fNum] = -1;
									}
									case 4: {
												if (name.find(pos) != string::npos) labels40[fNum] = 1;
												else								labels40[fNum] = -1;
									}
									case 10: {
												if (name.find(pos) != string::npos) labels100[fNum] = 1;
												else								labels100[fNum] = -1;
									}
									default: {
										cout << "Data number not supported" << endl;
										break;
									}

									}
									fNum++;
								} while (FindNextFile(hFind, &data) != 0 && (fNum < 2*dataNum*10));   //continue until there are no more files
								FindClose(hFind);
							}

							//setting up the parameters
							CvSVMParams params;
							params.svm_type = CvSVM::C_SVC;
							params.kernel_type = CvSVM::LINEAR;
							params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

							cout << "Training your SVM.." << endl;

							//training
							CvSVM SVM;

							switch (dataNum) {
								//fill a labels mat with 1 for every positive and -1 for every negative image
								case 1: {
									Mat labels_mat(fNum, 1, CV_32SC1, labels10);
									SVM.train(training_mat, labels_mat, Mat(), Mat(), params);
									SVM.save(picDir.c_str());
								}
								case 2: {
									Mat labels_mat(fNum, 1, CV_32SC1, labels20);
									SVM.train(training_mat, labels_mat, Mat(), Mat(), params);
									SVM.save(picDir.c_str());
								}
								case 3: {
									Mat labels_mat(fNum, 1, CV_32SC1, labels30);
									SVM.train(training_mat, labels_mat, Mat(), Mat(), params);
									SVM.save(picDir.c_str());
								}
								case 4: {
									Mat labels_mat(fNum, 1, CV_32SC1, labels40);
									SVM.train(training_mat, labels_mat, Mat(), Mat(), params);
									SVM.save(picDir.c_str());
								}
								case 10: {
									Mat labels_mat(fNum, 1, CV_32SC1, labels100);
									SVM.train(training_mat, labels_mat, Mat(), Mat(), params);
									SVM.save(picDir.c_str());
								}
								default: {
									cout << "Data number not supported" << endl; 
									break; 
								}

							}
							//test on sample data
							/*
							for (int i = 0; i < image.rows; ++i)
								for (int j = 0; j < image.cols; ++j)
								{
									Mat sampleMat = (Mat_<float>(1, 2) << j, i);
									float response = svm->predict(sampleMat);
									if (response == 1)
										image.at<Vec3b>(i, j) = green;
									else if (response == -1)
										image.at<Vec3b>(i, j) = blue;
								}
						    */
							//save the classifier
							//svm.save("svm_filename"); // saving
							//svm.load("svm_filename"); // loading
						}
						else {
							cout << "There need to be at least ten new positive images and 10 new negative images" << endl;
							cout << "to train your SVM" << endl; 
						}
					}
				}
			}
			picDir.erase(picDir.end() - dir.size() + 1, picDir.end());
			//write images to a folder
			SetCurrentDirectory(cDir.c_str());
			for (int i = 0; i < positives.size(); i++) {
				sprintf_s(pName, "pos%d.PNG", pNum++); 
				imwrite(pName, positives.at(i)); 
			}
			positives.erase(positives.begin(), positives.end()); 
			//SetCurrentDirectory(negDir.c_str());
			for (int i = 0; i < negatives.size(); i++) {
				sprintf_s(nName, "neg%d.PNG", nNum++);
				imwrite(nName, negatives.at(i));
			}
			negatives.erase(negatives.begin(), negatives.end());

		}
	}
	waitKey(0); 
	cout << "\nEnd of files" << endl;                                            
	return 0; 
}

