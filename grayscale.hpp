#ifndef GRAYSCALE_HPP
#define GRAYSCALE_HPP
#include <opencv2/opencv.hpp>

bool isGrayImage(cv::Mat img);

void grayscale_fullrange(cv::Mat &img);

void grayscale_splitrange(cv::Mat &img);


#endif 