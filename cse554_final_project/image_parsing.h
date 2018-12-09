#pragma once
#include <opencv2\opencv.hpp>
#include <cstdio>
#include <vector>


using namespace std;
using namespace cv;



void drawCircles(Mat &, const vector<Vec3f> &, const Scalar);
void measureRadius(Mat &input, Mat &output, vector<double> mean_thickness, const vector<Vec3f> &circles, int cell_thres, Mat &capsule_thres_image);