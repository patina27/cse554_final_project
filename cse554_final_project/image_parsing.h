#pragma once
#include<opencv2\opencv.hpp>
#include <cstdio>
#include <vector>


using namespace std;
using namespace cv;


void findCircles(const Mat &, vector<Vec3f> &, int, int, int);
void calcCircles(const Mat &, vector<Vec3f> &, int, int, int);
void drawCircles(Mat &, const vector<Vec3f> &);