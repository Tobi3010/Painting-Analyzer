#include "perspective.hpp"
#include "grayscale.hpp"
#include <random>
#include <vector>
#include <cmath>
#include <limits>







// ---------------------------------------------------------------------------------------


// Struct for line segment start- and end point
// And values for 'a' and 'p' for the line eqaution:
// x cos α + y sin α = p
struct eqLine {  
    // line segment
    cv::Point2f pt1;     // Start point
    cv::Point2f pt2;     // End point
    float len;           // Length

    // line equation of the form: x cos α + y sin α = p
    float a;
    float p;

    eqLine(){}

    eqLine(cv::Vec4f line) {
        pt1 = {line[0], line[1]};
        pt2 = {line[2], line[3]};
        len = cv::norm(pt1 - pt2);

        // Normal vector 
        float n_x = -(pt2.y - pt1.y);
        float n_y =  (pt2.x - pt1.x);
        float n_l = std::sqrt(n_x *n_x  + n_y*n_y);
        n_x = n_x / n_l;
        n_y = n_y / n_l;

        a = atan2(n_y, n_x);
        p = pt1.x * cos(a) + pt1.y * sin(a);
    } 
}; 

// Converts list of cv::Vec4f to list of eqLine
std::vector<eqLine> cvtLinesEqLines(std::vector<cv::Vec4f> &lines) {
    std::vector<eqLine> eqLines;
    for(cv::Vec4f line : lines) {
        eqLines.push_back(eqLine(line));
    }
    return eqLines;
}


void fltrVerticalHorizontal(std::vector<eqLine> &eqLines, float tolDeg){
    float tolRad = tolDeg * CV_PI / 180.0f; 
    std::vector<eqLine> filtered_eqLines;

    for (eqLine line : eqLines) {
        float alpha = line.a;
        if (alpha < 0) alpha += CV_PI;  // Normalize alpha to [0, π)

        if (std::fabs(alpha - 0) < tolRad || std::fabs(alpha - CV_PI) < tolRad) {
            continue; // Skip horizontal line
        }
        if (std::fabs(alpha - CV_PI/2) < tolRad) {
            continue; // Skip vetical line
        }
        filtered_eqLines.push_back(line);
    }
    eqLines = filtered_eqLines; 
}

void fltrLongest(std::vector<eqLine> &eqLines, float pct) {
    int n = eqLines.size() * pct;

    // Sort by length in descending order
    std::sort(eqLines.begin(), eqLines.end(), [](eqLine &a, eqLine &b) {
        return a.len > b.len; 
    });

    // Keep only the first n
    eqLines.resize(n);
}


void connectLines(std::vector<eqLine> &eqLines, float alpha_quant, float p_quant) {
    struct PointHash {
        std::size_t operator()(const cv::Point2f &p) const noexcept {
            auto h1 = std::hash<float>{}(p.x);
            auto h2 = std::hash<float>{}(p.y);
            return h1 ^ (h2 << 1);
        }
    };

    std::unordered_map<cv::Point2i, std::vector<eqLine>, PointHash> groups;
    for (eqLine& line : eqLines) {
        int qa = static_cast<int>(line.a * alpha_quant);   // quantize alpha in milliradians
        int qp = static_cast<int>(line.p * p_quant);       // quantize p in half pixels
        groups[{qa, qp}].push_back(line);
    }

    std::vector<eqLine> filtered_lines;
    for (auto &pair : groups) {
        if (pair.second.size() == 1) {
            filtered_lines.push_back(pair.second[0]);
        }
        else  {
            cv::Point2f maxStartPt = pair.second[0].pt1;
            cv::Point2f maxEndPt = pair.second[0].pt2;
            float maxLen = pair.second[0].len;
            int len = 0;
        
            for (eqLine line : pair.second) {
                len = cv::norm(line.pt1 - maxEndPt);
                if (len > maxLen) {
                    maxStartPt = line.pt1;
                    maxLen = len;
                }
                len = cv::norm(maxStartPt - line.pt2);
                if (len > maxLen) {
                    maxEndPt = line.pt2;
                    maxLen = len;
                }
            }
            filtered_lines.push_back(eqLine({
                maxStartPt.x, maxStartPt.y, 
                maxEndPt.x, maxEndPt.y})
            );
        }
    }
    eqLines = filtered_lines;

     /*
    int total = 0;
    for (auto &pair : groups) {
        if (pair.second.size() > 1) {
            printf("(%d, %d) : %d\n", pair.first.x, pair.first.y, pair.second.size());
            total += pair.second.size();
        }
    }
    printf("total %d\n", total);
    */
}


bool line_intersection(const eqLine &l1, const eqLine &l2, cv::Point2f &pt) {
    float x1 = l1.pt1.x, y1 = l1.pt1.y;
    float x2 = l1.pt2.x, y2 = l1.pt2.y;
    float x3 = l2.pt1.x, y3 = l2.pt1.y;
    float x4 = l2.pt2.x, y4 = l2.pt2.y;

    float a1 = y2 - y1;
    float b1 = x1 - x2;
    float c1 = a1 * x1 + b1 * y1;

    float a2 = y4 - y3;
    float b2 = x3 - x4;
    float c2 = a2 * x3 + b2 * y3;

    float det = a1 * b2 - a2 * b1;
    if (fabs(det) < 1e-6f) {
        return false; // Parallel
    }

    pt.x = (b2 * c1 - b1 * c2) / det;
    pt.y = (a1 * c2 - a2 * c1) / det;
    return true;
}


float find_dist_to_line(const cv::Point2f &pt, const eqLine &l) {
    float x0 = pt.x;
    float y0 = pt.y;
    float x1 = l.pt1.x, y1 = l.pt1.y;
    float x2 = l.pt2.x, y2 = l.pt2.y;

    float numerator = std::fabs((y2 - y1) * x0 - (x2 - x1) * y0 + x2 * y1 - y2 * x1);
    float denominator = std::sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));

    if (denominator < 1e-6f) return -1.0f; // invalid line

    return numerator / denominator;
}


cv::Point2f RANSAC(std::vector<eqLine> &eqLines, int ransac_iterations, float ransac_threshold, float ransac_ratio) {
    std::mt19937 rng(std::random_device{}());
    cv::Point2f vanishing_point(0, 0);
    float inlier_count_ratio = 0.0f;

    if (eqLines.size() < 2) return vanishing_point;

    std::uniform_int_distribution<int> dist_index(0, eqLines.size() - 1);

    for (int iteration = 0; iteration < ransac_iterations; ++iteration) {
        // Randomly sample two different lines
        int idx1 = dist_index(rng);
        int idx2 = dist_index(rng);
        while (idx2 == idx1) idx2 = dist_index(rng);

        cv::Point2f intersection_point;
        if (!line_intersection(eqLines[idx1], eqLines[idx2], intersection_point))
            continue;

        // Count inliers
        int inlier_count = 0;
        for (const auto &line : eqLines) {
            float d = find_dist_to_line(intersection_point, line);
            if (d < ransac_threshold) {
                inlier_count++;
            }
        }

        float ratio = static_cast<float>(inlier_count) / static_cast<float>(eqLines.size());
        if (ratio > inlier_count_ratio) {
            inlier_count_ratio = ratio;
            vanishing_point = intersection_point;
        }

        // Early stop if enough inliers
        if (inlier_count > static_cast<int>(eqLines.size() * ransac_ratio)) {
            break;
        }
    }

    return vanishing_point;
}


cv::Point2f perspective_lines(cv::Mat &img){
    if (img.empty()) { "No image"; return cv::Point2f(-1, -1); }

    // First: Apply edge detection using Canny Edge Detector ------
    cv::Mat img_gray, img_blur, img_edges;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(img_gray, img_blur, cv::Size(5, 5),2);    // Apply Gaussian blur
    cv::Canny(img_blur, img_edges, 80, 200);                    // Apply Canny Edge Detector
  
    // -------------------------------------------------------------

    // Second: Detect lines using ----------------------------------
    std::vector<cv::Vec4f> lines;
    cv::HoughLinesP(img_edges, lines, 1, CV_PI/180, 20, 10, 5);
    std::vector<eqLine> eqLines = cvtLinesEqLines(lines);

    fltrVerticalHorizontal(eqLines, 5.0);
    connectLines(eqLines, 2.0, 5.0);
    fltrLongest(eqLines, 1);
    
    //cv::cvtColor(img_edges, img, cv::COLOR_GRAY2BGR);
    for (eqLine line : eqLines) {
        cv::line(img, 
            line.pt1, 
            line.pt2, 
            cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
    }

    cv::Point2f vp = RANSAC(eqLines, 2000, 15.0f, 0.7f); // 2000 iterations, 5px threshold, 70% ratio
    cv::circle(img, vp, 7, cv::Scalar(0, 0, 255), -1);
    return vp;

}

