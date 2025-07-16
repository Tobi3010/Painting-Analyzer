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

// Turn image into grayscale with all shades (0-255)
cv::Mat grayscaleBasic (cv::Mat img)
{
    cvtColor(img, img, cv::COLOR_BGR2GRAY); // Turn image gray
    return img;
}

// Function to divide image into n shades of grayscale (
// Only binary (black and white) supported as of now
cv::Mat grayscaleShades(cv::Mat img)
{
    
    grayscaleBasic(img).copyTo(img); // Turn image into base grayscale
    int avg =  cv::mean(img)[0];     // Find average pixel value
    //std::cout << avg << "\n";
  
    // Loop through pixels 
    for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++) 
            if (img.at<uchar>(i,j) < avg){ // If pixel value is below average, turn it black
                img.at<uchar>(i,j) = 0;
            }
            else {                         // If pixel valu is above average, turn it white
                img.at<uchar>(i,j) = 255;  
            }
    return img;
}




