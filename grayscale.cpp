#include "grayscale.hpp"

// Returns true if grayscale, by checking BLUE = GREEN = RED
// Ensures it works no matter the number of channels.
bool isGrayImage(cv::Mat img) {
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
void grayscale_fullrange (cv::Mat &img) {
    if (img.empty()) { "No image"; return; }

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
}

// Function to divide image into n shades of grayscale 
// Only binary (black and white) supported as of now
void grayscale_splitrange(cv::Mat &img) {
    if (img.empty()) { "No image"; return; }
     
    grayscale_fullrange(img);       // Turn image into base grayscale
    int avg =  cv::mean(img)[0];    // Find average pixel value
  
    // Loop through pixels 
    for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++) 
            if (img.at<uchar>(i,j) < avg){ // If pixel value is below average, turn it black
                img.at<uchar>(i,j) = 0;
            }
            else {                         // If pixel valu is above average, turn it white
                img.at<uchar>(i,j) = 255;  
            }
}

// Turns image into grayscale with custom number of shades.
// Using color quantization (just gray) by KMeans method.
void grayscale_splitrange2(cv::Mat &img, int nShades, int seed = 42) {
    if (img.empty()) { "No image"; return; }
     
    cv::Mat data;
    grayscale_fullrange(img);                               // Turn image into base grayscale, one channel
    img.reshape(1, img.total()).convertTo(data, CV_32F);    // Reshapes the 2D image into a flat 1D column, convert to float because KMeans expects it
    
    cv::theRNG().state = seed;  // Ensures the output is the same for each run
    cv::Mat labels;             // Cluster labels for pixels
    cv::Mat centers;            // CLuster centers
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0);  // Kmeans stop criteria
    cv::kmeans(data, nShades, labels, criteria, 1, cv::KMEANS_PP_CENTERS, centers);          // Kmeans

    cv::Mat result(data.size(), CV_8U);          
    // Raw pointers for faster pixel access   
    uchar* result_ptr = result.ptr<uchar>();            // Raw pointer to of result image
    const float* center_ptr = centers.ptr<float>();     // The grayscale value of cluster i
    const int* label_ptr = labels.ptr<int>();           // The assigned cluster index for pixel i

    #pragma omp parallel for                // Parallel loop using OpenMP for speed.
    for (int i = 0; i < data.rows; ++i) {   // Each pixel is assigned its cluster center value 
        result_ptr[i] = static_cast<uchar>(center_ptr[label_ptr[i]]); 
    }

    img = result.reshape(1, img.rows);  // Reshape back to 2D
           
}



