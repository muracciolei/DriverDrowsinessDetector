#pragma once
#include <opencv2/opencv.hpp>
class CameraCapture {
public:
    CameraCapture(int device = 0);
    ~CameraCapture();
    bool isOpened() const;
    bool grabFrame(cv::Mat& frame);
private:
    cv::VideoCapture cap;
};