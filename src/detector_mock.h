#pragma once

#ifdef OPENCV_MOCK_BUILD
// Mock OpenCV types for Qt-only compilation
#include <string>
#include <vector>

namespace cv {
    // Forward declarations first
    struct Point;
    struct Size;
    
    struct Point {
        int x = 0, y = 0;
        Point() = default;
        Point(int x, int y) : x(x), y(y) {}
        Point operator+(const Point& other) const { return Point(x + other.x, y + other.y); }
    };
    
    struct Size {
        int width = 0, height = 0;
        Size() = default;
        Size(int w, int h) : width(w), height(h) {}
    };
    
    struct Rect {
        int x = 0, y = 0, width = 0, height = 0;
        Rect() = default;
        Rect(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}
        
        Point tl() const { return Point(x, y); }
        Size size() const { return Size(width, height); }
        int area() const { return width * height; }
    };
    
    struct Scalar {
        Scalar(int, int, int) {}
        Scalar() {}
    };
    
    class Mat {
    public:
        unsigned char* data = nullptr;
        int cols = 0;
        int rows = 0;
        size_t step = 0;
        int channels() const { return 3; }
        bool empty() const { return true; }
        Mat clone() const { return Mat(); }
        Mat operator()(const Rect&) const { return Mat(); }
    };
    
    class VideoCapture {
    public:
        bool open(int) { return false; }
        bool isOpened() const { return false; }
        void release() {}
        VideoCapture& operator>>(Mat&) { return *this; }
        double get(int) const { return 0.0; }
        bool set(int, double) { return false; }
    };
    
    class CascadeClassifier {
    public:
        bool load(const std::string&) { return false; }
        void detectMultiScale(const Mat& img, std::vector<Rect>& objects, 
                              double scaleFactor = 1.1, int minNeighbors = 3, 
                              int flags = 0, Size minSize = Size()) {}
    };
    
    // Declare but don't define these functions to avoid linker conflicts
    void rectangle(Mat&, Rect, Scalar, int);
    void cvtColor(const Mat&, Mat&, int);
    
    const int COLOR_BGR2GRAY = 0;
    const int CAP_PROP_FRAME_WIDTH = 0;
    const int CAP_PROP_FRAME_HEIGHT = 0;
    const int CAP_PROP_FPS = 0;
    const int CAP_PROP_BUFFERSIZE = 0;
}
#else
#include <opencv2/opencv.hpp>
#endif

#include <QString>
#include <vector>