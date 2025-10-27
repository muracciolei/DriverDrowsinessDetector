#pragma once

#ifdef OPENCV_MOCK_BUILD
#include "detector_mock.h"
#else
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#endif

#include <QString>
#include <QObject>

struct EyeStatus {
    double leftEAR = 0.0;
    double rightEAR = 0.0;
    bool drowsy = false;
};

class Detector : public QObject {
    Q_OBJECT
    
public:
    Detector(const QString& faceCascadePath, const QString& eyeCascadePath, QObject* parent = nullptr);
    ~Detector();
    bool isOpened() const;
    bool process(cv::Mat& renderFrame, EyeStatus& result);

private:
#ifdef OPENCV_MOCK_BUILD
    bool opened = true;
#else
    cv::VideoCapture cap;
    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyeCascade;
    bool opened = false;
    
    std::vector<cv::Rect> detectEyes(const cv::Mat& roi);
    double calculateSimplifiedEAR(const cv::Rect& eye);
#endif

signals:
    void detectionReady(const EyeStatus& status);
};