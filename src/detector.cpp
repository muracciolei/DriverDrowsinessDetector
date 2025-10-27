#include "detector.h"

#ifdef OPENCV_MOCK_BUILD
#include "utils.h"
#endif

Detector::Detector(const QString& faceCascadePath, const QString& eyeCascadePath, QObject* parent)
    : QObject(parent) {
#ifdef OPENCV_MOCK_BUILD
    Q_UNUSED(faceCascadePath)
    Q_UNUSED(eyeCascadePath)
    // Mock always "opened"
#else
    opened = cap.open(0)
        && faceCascade.load(faceCascadePath.toStdString())
        && eyeCascade.load(eyeCascadePath.toStdString());
    
    if (cap.isOpened()) {
        // Set camera properties for better performance
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(cv::CAP_PROP_FPS, 30);
    }
#endif
}

Detector::~Detector() { 
#ifndef OPENCV_MOCK_BUILD
    if (cap.isOpened()) cap.release(); 
#endif
}

bool Detector::isOpened() const { return opened; }

bool Detector::process(cv::Mat& renderFrame, EyeStatus& result) {
    result = EyeStatus();
    if (!opened) return false;

#ifdef OPENCV_MOCK_BUILD
    // Mock implementation - generate random values
    result.leftEAR = 0.25 + (rand() % 100) / 1000.0;
    result.rightEAR = 0.25 + (rand() % 100) / 1000.0;
    result.drowsy = result.leftEAR < 0.23 || result.rightEAR < 0.23;
    
    // Create a simple mock frame (empty matrix is enough for simulation)
    renderFrame = cv::Mat();
    
    emit detectionReady(result);
    return true;
#else
    // Real OpenCV implementation
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return false;
    
    renderFrame = frame.clone();
    std::vector<cv::Rect> faces;
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(80, 80));
    
    if (faces.empty()) {
        result.leftEAR = 0.0;
        result.rightEAR = 0.0;
        result.drowsy = true;
        emit detectionReady(result);
        return true;
    }
    
    auto face = faces[0];
    cv::rectangle(renderFrame, face, cv::Scalar(0, 255, 0), 2);
    cv::Mat faceROI = gray(face);
    std::vector<cv::Rect> eyes = detectEyes(faceROI);
    cv::Point faceOffset(face.x, face.y);
    
    std::vector<cv::Rect> sortedEyes = eyes;
    std::sort(sortedEyes.begin(), sortedEyes.end(), 
        [](const cv::Rect& a, const cv::Rect& b) { return a.x < b.x; });
    
    if (sortedEyes.size() >= 2) {
        cv::Rect left = sortedEyes[0];
        cv::Rect right = sortedEyes[1];
        result.leftEAR = calculateSimplifiedEAR(left);
        result.rightEAR = calculateSimplifiedEAR(right);
        cv::rectangle(renderFrame, cv::Rect(left.tl() + faceOffset, left.size()), cv::Scalar(255, 0, 0), 2);
        cv::rectangle(renderFrame, cv::Rect(right.tl() + faceOffset, right.size()), cv::Scalar(0, 0, 255), 2);
    } else {
        if (!sortedEyes.empty()) {
            result.leftEAR = calculateSimplifiedEAR(sortedEyes[0]);
            cv::rectangle(renderFrame, cv::Rect(sortedEyes[0].tl() + faceOffset, sortedEyes[0].size()), cv::Scalar(200, 0, 0), 2);
        }
    }
    
    double meanEAR = (result.leftEAR + result.rightEAR) / 2.0;
    static const double threshold = 0.23;
    static int drowsyFrames = 0;
    static int fps = 20;
    
    if (meanEAR > 0.0001 && meanEAR < threshold) {
        drowsyFrames++;
    } else {
        drowsyFrames = 0;
    }
    
    result.drowsy = drowsyFrames >= static_cast<int>(0.5 * fps);
    emit detectionReady(result);
    return true;
#endif
}

#ifndef OPENCV_MOCK_BUILD
std::vector<cv::Rect> Detector::detectEyes(const cv::Mat& roi) {
    std::vector<cv::Rect> eyes;
    eyeCascade.detectMultiScale(roi, eyes, 1.1, 12, 0, cv::Size(24, 24));
    return eyes;
}

double Detector::calculateSimplifiedEAR(const cv::Rect& eye) {
    // Simplified EAR calculation based on eye rectangle dimensions
    double ratio = static_cast<double>(eye.height) / static_cast<double>(eye.width);
    // Normalize to typical EAR range (0.2 - 0.4)
    double ear = ratio * 0.8;
    return std::max(0.1, std::min(0.5, ear));
}
#endif