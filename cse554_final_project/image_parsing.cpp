#include "stdafx.h"
#include "image_parsing.h"
#include <cmath>
#include <algorithm> 
#include <string>

#define PI CV_PI




void drawCircles(Mat &input, const vector<Vec3f> &circles, Scalar color) {
	for (int i = 0; i<circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(input, center, radius, color, 1, 8, 0);
	}
}

void measureRadius(Mat &input, Mat &output, vector<double> mean_thickness, const vector<Vec3f> &circles, int cell_thres, Mat &capsule_thres_image) {
	cvtColor(input, output, COLOR_GRAY2BGR);
	mean_thickness.clear();
	for (int m = 0; m < circles.size(); m++) {
		Point center(cvRound(circles[m][0]), cvRound(circles[m][1]));
		int radius = cvRound(circles[m][2]);
		vector<double> dists;
		for (int i = 0; i < 360; i += 15) {
			double s = sin(i*CV_PI / 180);
			double c = cos(i*CV_PI / 180);
			Point p2(cvRound(center.x + s * 3 * radius), cvRound(center.y + c * 3 * radius));
			LineIterator it(input, center, p2, 8);
			int upper_left_x = (center.x - 3 * radius > 0) ? (int)(center.x - 3 * radius) : 0;
			int upper_left_y = (center.y - 3 * radius > 0) ? (int)(center.y - 3 * radius) : 0;
			int lower_right_x = (center.x + 3 * radius < input.cols) ? (int)(center.x + 3 * radius) : input.cols;
			int lower_right_y = (center.y + 3 * radius < input.rows) ? (int)(center.y + 3 * radius) : input.rows;
			Rect bounds(upper_left_x, upper_left_y, lower_right_x - upper_left_x, lower_right_y - upper_left_y);
			Point inner(-1, -1);
			Point outer(-1, -1);
			while (bounds.contains(it.pos())) {
				uchar pixel = input.at<uchar>(it.pos());
				uchar bound = capsule_thres_image.at<uchar>(it.pos());
				if (inner.x == -1 && pixel < cell_thres) {
					inner.x = it.pos().x;
					inner.y = it.pos().y;
				}
				else {
					if (bound == 0) {
						outer.x = it.pos().x;
						outer.y = it.pos().y;
						break;
					}
				}
				++it;
			}
			if (inner.x == -1 || outer.x == -1)
				continue;
			line(output, inner, outer, Scalar(255, 0, 0));
			double dist = norm(outer - inner);
			dists.push_back(dist);
		}
		sort(dists.begin(), dists.end());
		int cnt = 0;
		double sum = 0;
		for (int i = dists.size() / 4; i <= 3 * dists.size() / 4; i++) {
			sum += dists[i];
			cnt++;
		}
		mean_thickness.push_back(sum / cnt);
		putText(output, to_string(sum / cnt), center, FONT_HERSHEY_PLAIN, 0.7, Scalar(255, 255, 0), 2);
	}
}

//
//double measureRadius(Mat &input, const vector<Vec3f> &circles, int cellThreshold = 128, int capsuleThreshold = 128) {
//	Mat cell_thresimg, capsule_thresimg;
//	double averageThickness = 0, totalThickness = 0;
//	for (int y = 0; y < input.rows; y++) {
//		for (int x = 0; x < input.cols; x++) {
//			if (input.at<uchar>(y, x) > cellThreshold) cell_thresimg.at<uchar>(y, x) = 255;
//			else cell_thresimg.at<uchar>(y, x) = 0;
//		}
//	}
//	for (int y = 0; y < input.rows; y++) {
//		for (int x = 0; x < input.cols; x++) {
//			if (input.at<uchar>(y, x) > capsuleThreshold) capsule_thresimg.at<uchar>(y, x) = 255;
//			else capsule_thresimg.at<uchar>(y, x) = 0;
//		}
//	}
//	for (int m = 0; m < circles.size(); m++) {
//		Point center(cvRound(circles[m][0]), cvRound(circles[m][1]));
//		int radius = cvRound(circles[m][2]);
//		int count = 0, totalCount = 0, totalEdge = 0;
//		double cellRadius = 0, capsuleRadius = 0;
//
//		while ((center.y + count) < input.rows && cell_thresimg.at<uchar>(center.y + count, center.x) == 255) {
//			count++;
//		}
//		if ((center.y + count) < input.rows) {
//			totalEdge++;
//			totalCount += (count - 1);
//		}
//		count = 0;
//		while ((center.x + count) < input.cols && cell_thresimg.at<uchar>(center.y, center.x + count) == 255) {
//			count++;
//		}
//		if ((center.x + count) < input.cols) {
//			totalEdge++;
//			totalCount += (count - 1);
//		}
//		count = 0;
//		while ((center.y - count) >= 0 && cell_thresimg.at<uchar>(center.y - count, center.x) == 255) {
//			count++;
//		}
//		if ((center.y - count) >= 0) {
//			totalEdge++;
//			totalCount += (count - 1);
//		}
//		count = 0;
//		while ((center.x - count) >= 0 && cell_thresimg.at<uchar>(center.y, center.x - count) == 255) {
//			count++;
//		}
//		if ((center.x - count) >= 0) {
//			totalEdge++;
//			totalCount += (count - 1);
//		}
//		cellRadius = totalCount * 1.0 / totalEdge;
//
//
//		int yPoint, xPoint;
//		vector<double> len;
//		double totalR = 0.0;
//		for (int i = 0; i < 60; i++) {
//			len.push_back(0.0);
//		}
//		for (int i = 0; i < 15; i++) {
//			if ((center.y + radius * sin(PI / 30 * i)) > input.rows) {
//				yPoint = input.rows;
//			}
//			else {
//				yPoint = center.y + radius * sin(PI / 30 * i);
//			}
//			if ((center.x + radius * cos(PI / 30 * i)) > input.cols) {
//				xPoint = input.cols;
//			}
//			else {
//				xPoint = center.x + radius * cos(PI / 30 * i);
//			}
//			Point pt(yPoint, xPoint);
//			LineIterator it(capsule_thresimg, center, pt);
//			bool isWhite = true;
//			count = 0;
//			for (int j = 0; j < it.count && isWhite; j++) {
//				Point p = it.pos();
//				if (capsule_thresimg.at<uchar>(p.y, p.x) == 0) {
//					isWhite = false;
//				}
//				count = j - 1;
//			}
//			len[i] = count * radius * 1.0 / it.count;
//		}
//		for (int i = 15; i < 30; i++) {
//			if ((center.y + radius * sin(PI / 30 * i)) > input.rows) {
//				yPoint = input.rows;
//			}
//			else {
//				yPoint = center.y + radius * sin(PI / 30 * i);
//			}
//			if ((center.x + radius * cos(PI / 30 * i)) <= 0) {
//				xPoint = 0;
//			}
//			else {
//				xPoint = center.x + radius * cos(PI / 30 * i);
//			}
//			Point pt(yPoint, xPoint);
//			LineIterator it(capsule_thresimg, center, pt);
//			bool isWhite = true;
//			count = 0;
//			for (int j = 0; j < it.count && isWhite; j++) {
//				Point p = it.pos();
//				if (capsule_thresimg.at<uchar>(p.y, p.x) == 0) {
//					isWhite = false;
//				}
//				count = j - 1;
//			}
//			len[i] = count * radius * 1.0 / it.count;
//		}
//		for (int i = 30; i < 45; i++) {
//			if ((center.y + radius * sin(PI / 30 * i)) <= 0) {
//				yPoint = 0;
//			}
//			else {
//				yPoint = center.y + radius * sin(PI / 30 * i);
//			}
//			if ((center.x + radius * cos(PI / 30 * i)) <= 0) {
//				xPoint = 0;
//			}
//			else {
//				xPoint = center.x + radius * cos(PI / 30 * i);
//			}
//			Point pt(yPoint, xPoint);
//			LineIterator it(capsule_thresimg, center, pt);
//			bool isWhite = true;
//			count = 0;
//			for (int j = 0; j < it.count && isWhite; j++) {
//				Point p = it.pos();
//				if (capsule_thresimg.at<uchar>(p.y, p.x) == 0) {
//					isWhite = false;
//				}
//				count = j - 1;
//			}
//			len[i] = count * radius * 1.0 / it.count;
//		}
//		for (int i = 45; i < 60; i++) {
//			if ((center.y + radius * sin(PI / 30 * i)) <= 0) {
//				yPoint = 0;
//			}
//			else {
//				yPoint = center.y + radius * sin(PI / 30 * i);
//			}
//			if ((center.x + radius * cos(PI / 30 * i)) > input.cols) {
//				xPoint = input.cols;
//			}
//			else {
//				xPoint = center.x + radius * cos(PI / 30 * i);
//			}
//			Point pt(yPoint, xPoint);
//			LineIterator it(capsule_thresimg, center, pt);
//			bool isWhite = true;
//			count = 0;
//			for (int j = 0; j < it.count && isWhite; j++) {
//				Point p = it.pos();
//				if (capsule_thresimg.at<uchar>(p.y, p.x) == 0) {
//					isWhite = false;
//				}
//				count = j - 1;
//			}
//			len[i] = count * radius * 1.0 / it.count;
//		}
//		sort(len.begin(), len.end());
//		for (int i = 6; i <= 15; i++) {
//			totalR += len[i];
//		}
//		capsuleRadius = totalR / 10;
//		totalThickness += (capsuleRadius - cellRadius);
//	}
//	averageThickness = totalThickness / circles.size();
//	return averageThickness;
//}