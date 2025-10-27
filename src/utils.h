#pragma once

#ifdef OPENCV_MOCK_BUILD
#include "detector_mock.h"
#else
#include <opencv2/opencv.hpp>
#endif

#include <cmath>

inline double euclideanDistance(const cv::Point& p1, const cv::Point& p2) {
    return std::hypot(p1.x - p2.x, p1.y - p2.y);
}

inline double computeEAR(const std::vector<cv::Point>& eye) {
    if (eye.size() == 6) {
        double A = euclideanDistance(eye[1], eye[5]);
        double B = euclideanDistance(eye[2], eye[4]);
        double C = euclideanDistance(eye[0], eye[3]);
        return (A + B) / (2.0 * C);
    }
    return 0.0;
}

inline double computeRectEAR(const cv::Rect& eye) {
    // Modern bbox approximation
    if (eye.width == 0) return 0.0;
    return std::max(0.0, static_cast<double>(eye.height) / static_cast<double>(eye.width));
}