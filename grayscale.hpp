#ifndef GRAYSCALE_HPP
#define GRAYSCALE_HPP
#include <opencv2/opencv.hpp>

// Declarations
bool isGrayImage(cv::Mat img);

void grayscaleBasic(cv::Mat &img);

void grayscaleShades(cv::Mat &img);


#endif 