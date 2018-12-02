// cse554_final_project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include<opencv2\opencv.hpp>
#include <cstdio>
#include <vector>

#include "image_parsing.h"

using namespace std;
using namespace cv;

int upper_thres = 128, lower_thres = 128;
Mat img, thres_img;
static void on_thres(int, void*) {
	cout << upper_thres << ' ' << lower_thres << endl;
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (img.at<uchar>(y, x) > upper_thres) thres_img.at<uchar>(y, x) = 255;
			if (img.at<uchar>(y, x) < lower_thres) thres_img.at<uchar>(y, x) = 0;
		}
	}
	imshow("Display window1", thres_img);
}

int main() {
	Mat tImg = imread("input.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	resize(tImg, img, Size(600,600));
	Mat result;
	cvtColor(img,result,COLOR_GRAY2BGR);
	blur(img, img, Size(3, 3));
	namedWindow("Display window0", WINDOW_AUTOSIZE);
	namedWindow("Display window1", 1);
	imshow("Display window0", img);
	thres_img = img;
	imshow("Display window1", thres_img);
	createTrackbar("upper_thres:", "Display window1", &upper_thres, 255, on_thres);
	createTrackbar("lower_thres:", "Display window1", &lower_thres, 255, on_thres);
	on_thres(upper_thres, 0);
	on_thres(lower_thres, 0);
	//adaptiveThreshold(img, img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 5, 5);
	//threshold(img, img, mean(img).val[0], 255, THRESH_TOZERO);
	//morphologyEx(img, img, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	//morphologyEx(img, img, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	/*namedWindow("Display window1", WINDOW_AUTOSIZE);
	imshow("Display window1", img);
	vector<Vec3f> circles;
	calcCircles(img, circles);
	cout << circles.size() << endl;
	cout << circles[0] << endl;
	drawCircle(result, circles);
	
	
	namedWindow("Display window2", WINDOW_AUTOSIZE);
	imshow("Display window2", result);
	*/
	waitKey(0);

	return 0;
}

