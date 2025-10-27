//==============================================================================
// main.cpp - Punto de entrada principal del Oculox Drowsiness Detector
// Arquitectura thread-safe moderna con Qt6
//==============================================================================

#include <QApplication>
#include "src/oculox_window.h"

int main(int argc, char* argv[]) {
    // Inicializar la aplicación Qt
    QApplication app(argc, argv);
    
    // Configurar propiedades de la aplicación para mejor gestión de recursos
    app.setApplicationName("Oculox Drowsiness Detector");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("Oculox");
    
    // Crear la ventana principal con arquitectura thread-safe
    OculoxWindow win;
    win.setWindowTitle("Oculox Drowsiness Detector (Modern UI)");
    win.resize(700, 480);
    
    // Centrar la ventana en la pantalla
    win.show();
    
    // Entrar en el bucle de eventos de Qt
    return app.exec();
}