#pragma once
#include <opencv2/opencv.hpp>
class FaceAnalyzer {
public:
    FaceAnalyzer(const std::string& cascadePath);
    bool isLoaded() const;
    bool detectEyes(const cv::Mat& faceROI, std::vector<cv::Rect>& eyes);
private:
    cv::CascadeClassifier eyeCascade;
};