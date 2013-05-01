#ifndef HUNGARIAN_H_
#define HUNGARIAN_H_

#include <vector>
#include <opencv2/opencv.hpp>

#include "asp.h"

using namespace std;

void hungarian(const cv::Mat& weights, vector<int>& colids, vector<int>& rowids);

#endif
