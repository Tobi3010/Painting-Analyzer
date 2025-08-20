#include <opencv2/opencv.hpp>
#include "perspective.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::pair<std::string, cv::Point2f>> pairs =
    {   
       {"1p-perspcv-img1.jpg", {690, 782}}, 
       {"1p-perspcv-img2.jpg", {287, 248}},  
       {"1p-perspcv-img3.jpg", {641, 400}}, 
       {"1p-perspcv-img4.jpg", {624, 852}},
       {"1p-perspcv-img5.jpg", {949, 662}}, 
       {"1p-perspcv-img6.jpg", {290, 730}},
       {"1p-perspcv-img7.jpg", {525, 695}},
       {"1p-perspcv-img8.jpg", {667, 760}}, 
       {"1p-perspcv-img9.jpg", {1015, 726}}, 
       {"1p-perspcv-img10.jpg", {1057, 865}},
       {"1p-perspcv-img11.jpg", {252, 914}}, 
       {"1p-perspcv-img12.jpg", {378, 448}}, 
       {"1p-perspcv-img13.jpg", {880, 294}},
       {"1p-perspcv-img14.jpg", {873, 609}},
       {"1p-perspcv-img15.jpg", {1038, 1310}},
       {"1p-perspcv-img16.jpg", {968, 876}},
       {"1p-perspcv-img17.jpg", {772, 194}},
       {"1p-perspcv-img18.jpg", {658, 558}},  
       {"1p-perspcv-img19.jpg", {146, 493}},
       {"1p-perspcv-img20.jpg", {573, 643}},
       {"1p-perspcv-img21.jpg", {968, 583}}, 
       {"1p-perspcv-img22.jpg", {827, 449}},
       {"1p-perspcv-img23.jpg", {1060, 566}},
       {"1p-perspcv-img24.jpg", {92, 490}}, 
       {"1p-perspcv-img25.jpg", {850, 500}},
       {"1p-perspcv-img26.jpg", {1028, 282}},
       {"1p-perspcv-img27.jpg", {1147, 415}},
       {"1p-perspcv-img28.jpg", {813, 553}},
       {"1p-perspcv-img29.jpg", {813, 522}},  
    };

    cv::Mat img;
    double avgLen = 0;
    int corrects = 0;
    for (auto pair : pairs) {
        img = cv::imread("../testPics/"+pair.first);
    
        cv::Point2f pt = perspective_lines(img);
        double len = cv::norm(pt-pair.second);
        printf("%s len : %.2f \n", pair.first.c_str(), len);
        avgLen += len;
        if (len < 10) {
            corrects += 1;
        }

        //cv::imshow("Displayed Image", img);
        //cv::waitKey(0);
    }

    avgLen /= pairs.size();
    printf("Average Length To Vanishing Point : %.2f  \n", avgLen);
    printf("Number Of Corrects : %d \n", corrects);
    
    return 0;
}


