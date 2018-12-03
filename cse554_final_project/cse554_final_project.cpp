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
int canny_upper_thres = 34, canny_lower_thres = 82, hough_distance = 16;
Mat img, thres_img, result;


static void on_thres(int, void*) {
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (img.at<uchar>(y, x) > upper_thres) thres_img.at<uchar>(y, x) = 255;
			else thres_img.at<uchar>(y, x) = 0;
		}
	}
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat dst_img = Mat::zeros(img.rows, img.cols, CV_8UC3);
	findContours(thres_img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	
	for (int index = 0; index >= 0; index = hierarchy[index][0]) {
		Scalar color(rand() & 255, rand() & 255, rand() & 255);
		drawContours(dst_img, contours, index, color, FILLED, 8, hierarchy);
	}
	for (int index = 0; index < contours.size(); index++) {
		if (contours[index].size() < 50) continue;
		if (hierarchy[index][3] == -1) continue;
		RotatedRect rRect = fitEllipseAMS(contours[index]);
		Point2f vertices[4];
		rRect.points(vertices);
		for (int i = 0; i < 4; i++)
			line(dst_img, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 2);
	}
	imshow("Display window1", thres_img);
	imshow("Display window2", dst_img);
}

static void on_canny(int, void*) {
	Mat canny_img;
	canny_img.create(img.size(), img.type());
	Canny(img, canny_img, canny_lower_thres, canny_upper_thres, 3);
	vector<Vec3f> circles;
	calcCircles(img, circles, canny_lower_thres, canny_upper_thres, hough_distance);
	cout << circles.size() << endl;
	cvtColor(img, result, COLOR_GRAY2BGR);
	drawCircles(result, circles);
	imshow("Display window3", canny_img);
	imshow("Display window0", result);
}

static void on_distance(int, void*) {
	vector<Vec3f> circles;
	calcCircles(img, circles, canny_lower_thres, canny_upper_thres, hough_distance);
	cout << circles.size() << endl;
	cvtColor(img, result, COLOR_GRAY2BGR);
	drawCircles(result, circles);
	imshow("Display window0", result);
}

int main() {
	Mat tImg = imread("input.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	resize(tImg, img, Size(600,600));
	thres_img = img.clone();

	cvtColor(img,result,COLOR_GRAY2BGR);
	blur(img, img, Size(5, 5));
	
	namedWindow("Display window0", WINDOW_AUTOSIZE);
	namedWindow("Display window1", WINDOW_AUTOSIZE);
	namedWindow("Display window2", WINDOW_AUTOSIZE); 
	namedWindow("Display window3", WINDOW_AUTOSIZE);
	


	createTrackbar("upper_thres:", "Display window3", &canny_upper_thres, 255, on_canny);
	createTrackbar("lower_thres:", "Display window3", &canny_lower_thres, 255, on_canny);
	createTrackbar("distance:", "Display window3", &hough_distance, 100, on_distance);
	imshow("Display window0", img);
	createTrackbar("upper_thres:", "Display window1", &upper_thres, 255, on_thres);
	on_thres(upper_thres, 0);
	on_canny(canny_upper_thres, 0);
	waitKey(0);

	return 0;
}

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