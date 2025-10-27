#include "face_analyzer.h"
FaceAnalyzer::FaceAnalyzer(const std::string& cascadePath) {
    eyeCascade.load(cascadePath);
}
bool FaceAnalyzer::isLoaded() const {
    return !eyeCascade.empty();
}
bool FaceAnalyzer::detectEyes(const cv::Mat& faceROI, std::vector<cv::Rect>& eyes) {
    if (!isLoaded()) return false;
    eyeCascade.detectMultiScale(faceROI, eyes, 1.1, 10, 0, cv::Size(24,24));
    return !eyes.empty();
}