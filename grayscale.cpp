#include "grayscale.hpp"




// returns true if grayscale, by checking BLUE = GREEN = RED
bool isGrayImage(cv::Mat img) 
{
    cv::Mat dst;
    cv::Mat bgr[3];
    split(img, bgr);
    absdiff(bgr[0], bgr[1], dst);

    if (countNonZero(dst))
        return false;

    absdiff(bgr[0], bgr[2], dst);
    return !countNonZero(dst);
}

cv::Mat grayscaleBasic (cv::Mat img)
{
    cv::Mat imgModified;
    cvtColor(img, imgModified, cv::COLOR_BGR2GRAY);
    cvtColor(imgModified, imgModified, cv::COLOR_GRAY2BGR);
    return imgModified;
}




