#include "stdafx.h"

#include<opencv2\opencv.hpp>
#include <cstdio>
#include <vector>

#include "image_parsing.h"

using namespace std;
using namespace cv;

Mat original_scaled_image;
Mat annotated_image;
Mat thres_image;
Mat blurred_image;
Mat cell_thres_image;
Mat capsule_thres_image;
Mat contour_thres_image;
Mat circles_annotated_image;
Mat thickness_annotated_image;
int cell_thres;
int capsule_thres;
int contour_thres;
int max_radius = 30;
int min_radius = 3;
int min_distance = 15;
int canny_upper_thres = 82;
int canny_lower_thres = 34;

vector<Vec3f> contour_circles;
vector<Vec3f> canny_circles;
vector<double> mean_thickness;

void on_thres_change(int, void*) {
	cvtColor(blurred_image, annotated_image, COLOR_GRAY2BGR);
	int n = annotated_image.rows, m = annotated_image.cols;
	
	threshold(blurred_image, capsule_thres_image, capsule_thres, 255, 0);\
	morphologyEx(capsule_thres_image, capsule_thres_image, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
	cell_thres_image = capsule_thres_image.clone();
	for (int y = 0; y < n; y++) {
		for (int x = 0; x < m; x++) {
			if (capsule_thres_image.at<uchar>(y, x) == 255) {
				annotated_image.at<Vec3b>(y, x) = 
					annotated_image.at<Vec3b>(y, x) * 0.6 + Vec3b(255, 0, 0) * 0.4;
				if (original_scaled_image.at<uchar>(y, x) < cell_thres) {
					cell_thres_image.at<uchar>(y, x) = 255; 
					annotated_image.at<Vec3b>(y, x) = 
						annotated_image.at<Vec3b>(y, x) * 0.5 + Vec3b(0, 0, 255) * 0.5;
				}
				else {
					cell_thres_image.at<uchar>(y, x) = 0;
				}
			}
		}
	}

	imshow("Display Window 2", annotated_image);
}

void on_canny(int, void*) {
	Mat canny_img;
	canny_img.create(blurred_image.size(), blurred_image.type());
	Canny(blurred_image, canny_img, canny_lower_thres, canny_upper_thres, 3);
	vector<Vec3f> circles;
	HoughCircles(blurred_image, circles, CV_HOUGH_GRADIENT, 1, 
		min_distance, canny_upper_thres, canny_lower_thres, min_radius, max_radius);
	canny_circles.clear();
	if (!contour_circles.empty()) {
		for (int i = 0; i < circles.size(); i++) {
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			double mind = -1;
			for (int i = 0; i < contour_circles.size(); i++) {
				Point c_center(cvRound(contour_circles[i][0]), cvRound(contour_circles[i][1]));
				if (mind == -1 || norm(center - c_center) < mind) {
					mind = norm(center - c_center);
				}
			}
			if (mind >= min_distance) {
				canny_circles.push_back(circles[i]);
			}
		}
	}
	cvtColor(original_scaled_image, circles_annotated_image, COLOR_GRAY2BGR);
	drawCircles(circles_annotated_image, contour_circles, Scalar(0, 0, 255));
	drawCircles(circles_annotated_image, canny_circles, Scalar(0, 255, 0));
	imshow("Display Window 1", circles_annotated_image);
}

void on_contour(int, void*) {
	threshold(blurred_image, contour_thres_image, contour_thres, 255, 0);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int n = blurred_image.rows, m = blurred_image.cols;
	Mat contour_image = Mat::zeros(n, m, CV_8UC3);
	findContours(contour_thres_image, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	for (int index = 0; index >= 0; index = hierarchy[index][0]) {
		Scalar color(rand() & 255, rand() & 255, rand() & 255);
		drawContours(contour_image, contours, index, color, FILLED, 8, hierarchy);
	}
	contour_circles.clear();
	for (int index = 0; index < contours.size(); index++) {
		if (contours[index].size() < 20) continue;
		RotatedRect rRect = fitEllipseAMS(contours[index]);
		Point2f vertices[4];
		rRect.points(vertices);
		double len1 = norm(vertices[0] - vertices[1]), len2 = norm(vertices[1] - vertices[2]);
		double radius = (len1 + len2) / 4;
		Point center(cvRound((vertices[0].x + vertices[2].x) / 2), cvRound((vertices[0].y + vertices[2].y) / 2));
		if (len1 / len2 > 1.2 || len2 / len1 > 1.2 || radius > max_radius || radius < min_radius) {
			continue;
		}
		contour_circles.push_back(Vec3f(center.x, center.y, radius));
	}
	on_canny(1, NULL);
}

void on_MouseHandle(int event, int x, int y, int flags, void *) {
	if (event == EVENT_LBUTTONDBLCLK) {
		vector<Vec3f> all_circles;
		all_circles.reserve(contour_circles.size() + canny_circles.size()); // preallocate memory
		all_circles.insert(all_circles.end(), contour_circles.begin(), contour_circles.end());
		all_circles.insert(all_circles.end(), canny_circles.begin(), canny_circles.end());
		measureRadius(original_scaled_image, thickness_annotated_image, mean_thickness, all_circles, cell_thres, capsule_thres_image);
		imshow("Display Window 3", thickness_annotated_image);
	}
}
int main(int argc, char** argv) {
	int blur_param = 0;
	if (argc < 2) {
		original_scaled_image = imread("input.jpg", CV_LOAD_IMAGE_GRAYSCALE);
		blur_param = 5;
	}
	else {
		if (argc == 3) {
			original_scaled_image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
			blur_param = atoi(argv[2]);
		}
		else {
			cout << "invalid input! Please try again!" << endl;
			return -1;
		}
	}
	contour_thres = (int)mean(original_scaled_image).val[0];
	cell_thres = contour_thres + 10;
	capsule_thres = contour_thres + 20;
	resize(original_scaled_image, original_scaled_image, 
		Size(600, original_scaled_image.cols * 600 / original_scaled_image.rows));
	thres_image = original_scaled_image.clone();
	blur(original_scaled_image, blurred_image, Size(blur_param, blur_param));
	namedWindow("Display Window 1", WINDOW_AUTOSIZE);
	namedWindow("Display Window 2", WINDOW_AUTOSIZE);
	namedWindow("Display Window 3", WINDOW_AUTOSIZE);
	setMouseCallback("Display Window 1", on_MouseHandle);
	imshow("Display Window 1", original_scaled_image);
	createTrackbar("Thres cell:", "Display Window 2", 
		           &cell_thres, 255, on_thres_change);
	createTrackbar("Thres capsule:", "Display Window 2",
		           &capsule_thres, 255, on_thres_change);
	createTrackbar("Thres contour:", "Display Window 1",
		           &contour_thres, 255, on_contour);
	createTrackbar("upper_thres:", "Display Window 1", &canny_upper_thres, 255, on_canny);
	createTrackbar("lower_thres:", "Display Window 1", &canny_lower_thres, 255, on_canny);
	createTrackbar("min_distance:", "Display Window 1", &min_distance, 100, on_canny);
	on_thres_change(1, NULL);
	on_contour(contour_thres, NULL);
	on_thres_change(cell_thres, NULL);
//	createTrackbar("Thres capsule:", "Display Window 2", &capsule_thres, 255, on_thres_change);
	waitKey(0);
	

	return 0;
}