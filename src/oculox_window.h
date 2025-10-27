#pragma once
#include <QMainWindow>
#include <QThread>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QStatusBar>
#include <QElapsedTimer>
#include <QMutex>
#include "detector.h"

class ProcessingThread : public QThread {
    Q_OBJECT
public:
    ProcessingThread(Detector* detector, QObject* parent = nullptr);
    void stop();

protected:
    void run() override;

signals:
    void frameProcessed(const QPixmap& pixmap, const QString& statusText, bool isDrowsy);

private:
    Detector* detector;
    QMutex stopMutex;
    bool shouldStop;
};

class OculoxWindow : public QMainWindow {
    Q_OBJECT
public:
    OculoxWindow(QWidget* parent = nullptr);
    ~OculoxWindow();

private slots:
    void updateFrame(const QPixmap& pixmap, const QString& statusText, bool isDrowsy);
    void updateEARStatus(double leftEAR, double rightEAR, bool isDrowsy);
    void simulateEARValues(); // For demo purposes

private:
    QLabel* videoLabel;
    QLabel* scoreLabel;
    QLabel* fpsLabel;
    
    // Status bar components
    QStatusBar* statusBar;
    QProgressBar* earProgressBar;
    QLabel* earValueLabel;
    QLabel* riskStatusLabel;
    QLabel* frameCountLabel;
    
    Detector* detector;
    ProcessingThread* processingThread;
    QElapsedTimer fpsTimer;
    QTimer* simulationTimer;
    
    int frameCount;
    double lastFPS;
    bool isDrowsinessActive;
    
    void setupStatusBar();
    void setupSimulation();
    void playAlertSound();
    void updateFPSCounter();
};