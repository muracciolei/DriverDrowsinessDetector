#include "drowsiness_analyzer.h"
DrowsinessAnalyzer::DrowsinessAnalyzer(double threshold, double period, int fps)
    : m_threshold(threshold), m_period(period), m_fps(fps), m_framesUnder(0) {}
bool DrowsinessAnalyzer::isDrowsy(const std::vector<double>& ears) {
    double avg = 0.0;
    for (double ear : ears) avg += ear;
    avg /= ears.size();
    if (avg < m_threshold && avg > 0.0) m_framesUnder++;
    else m_framesUnder = 0;
    return m_framesUnder >= int(m_period * m_fps);
}