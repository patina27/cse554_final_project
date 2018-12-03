#include "stdafx.h"
#include "image_parsing.h"

void findCircles(const Mat& input, vector<Vec3f> &circles, int param1, int param2, int distance=10) {
	Mat t_img;
	blur(input, t_img, Size(3, 3));
	calcCircles(t_img, circles, param1, param2, distance);
}

void calcCircles(const Mat &input, vector<Vec3f> &circles, int param1, int param2, int distance=10) {
	HoughCircles(input, circles, CV_HOUGH_GRADIENT, 1, distance, param1, param2, 1, 50);
}

void drawCircles(Mat &input, const vector<Vec3f> &circles) {
	for (int i = 0; i<circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(input, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}
}