#pragma once

#ifdef OPENCV_MOCK_BUILD
#include "detector_mock.h"
#else
#include <opencv2/opencv.hpp>
#endif

#include <cmath>

// Computes Euclidean distance between two cv::Point
inline double euclideanDistance(const cv::Point& p1, const cv::Point& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

// Calculates Eye Aspect Ratio given 6 landmarks (OpenCV Haar won't provide this, so will use bbox approximation)
inline double computeEAR(const std::vector<cv::Point>& eye) {
    // Standard EAR formula, override if only rectangle available
    if (eye.size() == 6) {
        double A = euclideanDistance(eye[1], eye[5]);
        double B = euclideanDistance(eye[2], eye[4]);
        double C = euclideanDistance(eye[0], eye[3]);
        return (A + B) / (2.0 * C);
    }
    return 0.0;
}

// Approximation if only rect:
inline double computeRectEAR(const cv::Rect& eye) {
    return static_cast<double>(eye.height) / static_cast<double>(eye.width);
}