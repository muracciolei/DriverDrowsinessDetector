#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
class DrowsinessAnalyzer {
public:
    DrowsinessAnalyzer(double threshold=0.23, double period=0.5, int fps=20);
    bool isDrowsy(const std::vector<double>& ears);
private:
    double m_threshold;
    double m_period;
    int m_fps;
    int m_framesUnder;
};