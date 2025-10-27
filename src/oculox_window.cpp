#include "oculox_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QSoundEffect>
#include <QStatusBar>
#include <QDateTime>
#include <QApplication>
#include <QSplitter>
#include <QFile>
#include <QUrl>
#include <QMutexLocker>

// ProcessingThread Implementation
ProcessingThread::ProcessingThread(Detector* detector, QObject* parent)
    : QThread(parent), detector(detector), shouldStop(false) {}

void ProcessingThread::stop() {
    QMutexLocker locker(&stopMutex);
    shouldStop = true;
}

void ProcessingThread::run() {
    const int targetFPS = 30;
    const int frameDelay = 1000 / targetFPS; // ms
    
    while (true) {
        {
            QMutexLocker locker(&stopMutex);
            if (shouldStop) break;
        }
        
        cv::Mat frame;
        EyeStatus status;
        
        if (detector && detector->process(frame, status)) {
            QPixmap pixmap;
            
#ifdef OPENCV_MOCK_BUILD
            // Create a demo pixmap (placeholder)
            pixmap = QPixmap(640, 480);
            pixmap.fill(Qt::darkGray);
#else
            // Convert OpenCV Mat to QPixmap
            if (!frame.empty()) {
                cv::Mat display;
                if (frame.channels() == 3) {
                    cv::cvtColor(frame, display, cv::COLOR_BGR2RGB);
                } else if (frame.channels() == 1) {
                    cv::cvtColor(frame, display, cv::COLOR_GRAY2RGB);
                } else {
                    display = frame;
                }
                
                QImage qimg(display.data, display.cols, display.rows, 
                           display.step, QImage::Format_RGB888);
                pixmap = QPixmap::fromImage(qimg);
            } else {
                pixmap = QPixmap(640, 480);
                pixmap.fill(Qt::darkGray);
            }
#endif
            
            QString statusText = QString("EAR: L=%.3f R=%.3f").arg(status.leftEAR).arg(status.rightEAR);
            
            emit frameProcessed(pixmap, statusText, status.drowsy);
        } else {
            emit frameProcessed(QPixmap(), "EAR: -- (No camera)", false);
        }
        
        msleep(frameDelay);
    }
}

OculoxWindow::OculoxWindow(QWidget* parent)
    : QMainWindow(parent), frameCount(0), lastFPS(0), isDrowsinessActive(false) {
    
    setWindowTitle("Oculox Drowsiness Detector - Status Bar");
    resize(720, 580);
    
    // Central widget and main layout
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    
    // Create UI components
    videoLabel = new QLabel("Video feed will appear here");
    videoLabel->setMinimumSize(640, 480);
    videoLabel->setStyleSheet("border: 2px solid gray; background-color: #2b2b2b; color: white;");
    videoLabel->setAlignment(Qt::AlignCenter);
    
    scoreLabel = new QLabel("EAR: --");
    scoreLabel->setStyleSheet("font-size: 14px; color: #00ff00;");
    
    fpsLabel = new QLabel("FPS: --");
    fpsLabel->setStyleSheet("font-size: 12px; color: #888;");
    
    frameCountLabel = new QLabel("Frames: 0");
    frameCountLabel->setStyleSheet("font-size: 12px; color: #888;");
    
    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    
    QHBoxLayout* infoRow = new QHBoxLayout();
    infoRow->addWidget(scoreLabel);
    infoRow->addStretch();
    infoRow->addWidget(frameCountLabel);
    infoRow->addWidget(fpsLabel);
    
    mainLayout->addWidget(videoLabel, 4);
    mainLayout->addLayout(infoRow, 0);
    
    // Setup status bar and simulation
    setupStatusBar();
    setupSimulation();
    
    // Initialize detector and processing thread with real Haar cascades
    detector = new Detector("resources/haarcascade_frontalface_alt.xml", 
                           "resources/haarcascade_eye_tree_eyeglasses.xml", 
                           this);
    processingThread = new ProcessingThread(detector, this);
    
    connect(processingThread, &ProcessingThread::frameProcessed,
            this, &OculoxWindow::updateFrame);
    
    // Start processing
    processingThread->start();
    fpsTimer.start();
}

OculoxWindow::~OculoxWindow() {
    if (processingThread) {
        processingThread->stop();
        processingThread->wait(3000);
        delete processingThread;
    }
}

void OculoxWindow::setupStatusBar() {
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    // Create status bar components
    earProgressBar = new QProgressBar();
    earProgressBar->setRange(0, 100);
    earProgressBar->setValue(25);
    earProgressBar->setTextVisible(true);
    earProgressBar->setFormat("EAR: %p%");
    earProgressBar->setMaximumWidth(150);
    
    earValueLabel = new QLabel("EAR: 0.25");
    earValueLabel->setMinimumWidth(80);
    
    riskStatusLabel = new QLabel("NORMAL");
    riskStatusLabel->setMinimumWidth(80);
    riskStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    
    // Add to status bar
    statusBar->addWidget(earValueLabel);
    statusBar->addWidget(earProgressBar);
    statusBar->addWidget(riskStatusLabel);
    statusBar->showMessage("Sistema iniciado - Modo demostración");
}

void OculoxWindow::setupSimulation() {
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &OculoxWindow::simulateEARValues);
    simulationTimer->start(200); // Update every 200ms for demo
}

void OculoxWindow::updateFrame(const QPixmap& pixmap, const QString& statusText, bool isDrowsy) {
    if (!pixmap.isNull()) {
        videoLabel->setPixmap(pixmap.scaled(videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    
    scoreLabel->setText(statusText);
    frameCount++;
    frameCountLabel->setText(QString("Frames: %1").arg(frameCount));
    
    updateFPSCounter();
    
    if (isDrowsy != isDrowsinessActive) {
        isDrowsinessActive = isDrowsy;
        if (isDrowsy) {
            playAlertSound();
        }
    }
}

void OculoxWindow::updateEARStatus(double leftEAR, double rightEAR, bool isDrowsy) {
    double avgEAR = (leftEAR + rightEAR) / 2.0;
    int progressValue = static_cast<int>(avgEAR * 100);
    
    earProgressBar->setValue(progressValue);
    earValueLabel->setText(QString("EAR: %.3f").arg(avgEAR));
    
    // Update colors based on risk level
    QString style;
    QString riskText;
    
    if (isDrowsy || avgEAR < 0.23) {
        style = "QProgressBar::chunk { background-color: #ff4444; } QProgressBar { color: white; }";
        riskText = "ALERTA";
        riskStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    } else if (avgEAR < 0.26) {
        style = "QProgressBar::chunk { background-color: #ffaa00; } QProgressBar { color: white; }";
        riskText = "PRECAUCIÓN";
        riskStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    } else {
        style = "QProgressBar::chunk { background-color: #44ff44; } QProgressBar { color: white; }";
        riskText = "NORMAL";
        riskStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    }
    
    earProgressBar->setStyleSheet(style);
    riskStatusLabel->setText(riskText);
}

void OculoxWindow::simulateEARValues() {
    // Generate realistic EAR values for demonstration
    static double time = 0;
    time += 0.2;
    
    // Simulate drowsiness cycles
    double baseEAR = 0.30;
    double variation = 0.05 * sin(time * 0.5);
    double drowsinessEffect = 0;
    
    // Occasional drowsiness simulation
    if (fmod(time, 10.0) < 2.0) {
        drowsinessEffect = -0.08 * sin(time * 3.0);
    }
    
    double leftEAR = baseEAR + variation + drowsinessEffect + (rand() % 20 - 10) / 1000.0;
    double rightEAR = baseEAR + variation + drowsinessEffect + (rand() % 20 - 10) / 1000.0;
    
    leftEAR = qMax(0.15, qMin(0.40, leftEAR));
    rightEAR = qMax(0.15, qMin(0.40, rightEAR));
    
    bool isDrowsy = leftEAR < 0.23 || rightEAR < 0.23;
    
    updateEARStatus(leftEAR, rightEAR, isDrowsy);
}

void OculoxWindow::updateFPSCounter() {
    static int framesSinceLastUpdate = 0;
    static qint64 lastTime = 0;
    
    framesSinceLastUpdate++;
    qint64 currentTime = fpsTimer.elapsed();
    
    if (currentTime - lastTime >= 1000) { // Update every second
        lastFPS = framesSinceLastUpdate * 1000.0 / (currentTime - lastTime);
        fpsLabel->setText(QString("FPS: %1").arg(lastFPS, 0, 'f', 1));
        framesSinceLastUpdate = 0;
        lastTime = currentTime;
    }
}

void OculoxWindow::playAlertSound() {
    static QSoundEffect alertSound;
    const QString alertPath = QStringLiteral("resources/alert.wav");

    // Verify file exists; if not, notify user non-intrusively via status bar
    if (!QFile::exists(alertPath)) {
        if (statusBar) {
            statusBar->showMessage(QString::fromUtf8("(¡Falta el archivo de alerta sonora: alert.wav!)"), 2500);
        }
        return;
    }

    // Lazy initialize the sound effect once
    if (alertSound.source().isEmpty()) {
        alertSound.setSource(QUrl::fromLocalFile(alertPath));
        alertSound.setLoopCount(1);           // play once
        alertSound.setVolume(0.9f);           // 90% volume
    }

    // Play only if not already playing to avoid overlap; non-blocking for UI
    if (!alertSound.isPlaying()) {
        alertSound.play();
    }
}