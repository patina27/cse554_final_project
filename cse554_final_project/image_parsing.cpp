#include "stdafx.h"
#include "image_parsing.h"

#include<opencv2\opencv.hpp>
#include <cstdio>
#include <vector>

using namespace std;
using namespace cv;


static void findCircles(const Mat& input, vector<Vec3f> &circles) {
	Mat t_img;
	blur(input, t_img, Size(3, 3));
	calcCircles(t_img, circles);
}

static void calcCircles(const Mat &input, vector<Vec3f> &circles) {
	HoughCircles(input, circles, CV_HOUGH_GRADIENT, 1, 10, 80, 65, 3, 50);
}

static void drawCircle(Mat &input, const vector<Vec3f> &circles) {
	for (int i = 0; i<circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(input, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}
}