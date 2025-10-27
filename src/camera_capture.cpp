#include "camera_capture.h"

CameraCapture::CameraCapture(int device) : cap(device) {}

CameraCapture::~CameraCapture() {
    if (cap.isOpened()) cap.release();
}

bool CameraCapture::isOpened() const {
    return cap.isOpened();
}

bool CameraCapture::grabFrame(cv::Mat& frame) {
    if (!cap.isOpened()) return false;
    cap >> frame;
    return !frame.empty();
}