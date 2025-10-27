# 🎯 Oculox Drowsiness Detector

[![Latest Release](https://img.shields.io/github/v/release/muracciolei/DriverDrowsinessDetector?style=for-the-badge&logo=github&logoColor=white&labelColor=2b2b2b&color=blue)](https://github.com/muracciolei/DriverDrowsinessDetector/releases/latest)
[![Download](https://img.shields.io/github/downloads/muracciolei/DriverDrowsinessDetector/total?style=for-the-badge&logo=download&logoColor=white&labelColor=2b2b2b&color=green)](https://github.com/muracciolei/DriverDrowsinessDetector/releases/latest)
[![License](https://img.shields.io/github/license/muracciolei/DriverDrowsinessDetector?style=for-the-badge&logo=opensourceinitiative&logoColor=white&labelColor=2b2b2b&color=orange)](https://github.com/muracciolei/DriverDrowsinessDetector/blob/main/LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%2010%2F11-blue?style=for-the-badge&logo=windows&logoColor=white&labelColor=2b2b2b)](https://github.com/muracciolei/DriverDrowsinessDetector/releases)

## 📋 Descripción

Sistema avanzado de detección de somnolencia para conductores utilizando análisis en tiempo real del Eye Aspect Ratio (EAR) y alertas visuales y sonoras integradas.

## 🚀 Descarga Rápida

### ⬇️ **Versión Lista para Usar (Recomendada)**

[![Download Release](https://img.shields.io/badge/📥%20Descargar%20v2.0.0-Listo%20para%20Usar-success?style=for-the-badge&logo=download)](https://github.com/muracciolei/DriverDrowsinessDetector/releases/download/v2.0.0/Oculox-DrowsinessDetector-v2.0.0-Windows-x64.zip)

**🎯 Instalación instantánea:**
1. **Descargar** el archivo ZIP (41.4 MB)
2. **Extraer** todos los archivos en una carpeta
3. **Ejecutar** `Oculox_DrowsinessDetector_StatusBar.exe`
4. **¡Listo!** - Comienza a detectar somnolencia inmediatamente

> ✅ **Sin compilación necesaria** - Todas las dependencias incluidas  
> ✅ **Windows 10/11 compatible** - 64-bit  
> ✅ **Completamente portable** - No requiere instalación

---

## ✨ Características Principales

- **🎥 Detección en tiempo real**: Análisis continuo del EAR usando OpenCV 4.11.0
- **⚠️ Alertas inteligentes**: Sistema visual (colores) y sonoro integrado
- **🖥️ UI moderna**: Interfaz Qt6 con video en vivo a 30 FPS
- **👁️ Precisión avanzada**: Haar Cascade para detección facial y ocular
- **🧵 Arquitectura robusta**: Procesamiento multi-hilo y modular
- **📦 Deployment completo**: Versión portable lista para usar

## Requisitos del Sistema

### Dependencias
- Qt6 (Core, Widgets, Multimedia)
- OpenCV 4.x
- CMake 3.16+
- C++17 compatible compiler

### Recursos Necesarios

#### 1. Archivo de Clasificador Haar
```
resources/haarcascade_eye.xml
```
**Fuente**: OpenCV contrib repository

#### 2. Archivo de Audio de Alerta
```
resources/alert.wav
```

**Especificaciones del archivo de audio:**
- Formato: WAV (Waveform Audio File Format)
- Codificación: PCM 16-bit
- Frecuencia de muestreo: 44.1 kHz
- Canales: Mono o Estéreo
- Duración recomendada: 1-3 segundos
- Volumen: Normalizado (no excesivamente alto)

**Instrucciones para obtener/crear alert.wav:**

1. **Opción 1 - Generar con herramientas online:**
   - Visitar freesound.org o zapsplat.com
   - Buscar "alert sound" o "warning beep"
   - Descargar en formato WAV con las especificaciones mencionadas

2. **Opción 2 - Crear con Audacity:**
   ```bash
   # Instalar Audacity
   sudo apt install audacity  # Linux
   # O descargar desde https://www.audacityteam.org/
   
   # Pasos en Audacity:
   # 1. Generar -> Tono -> Frecuencia 800Hz, 2 segundos
   # 2. Archivo -> Exportar -> Exportar como WAV
   # 3. Configurar: PCM signed 16-bit, 44100 Hz
   ```

3. **Opción 3 - Convertir archivo existente:**
   ```bash
   # Usando ffmpeg
   ffmpeg -i input_audio.mp3 -acodec pcm_s16le -ar 44100 -ac 1 alert.wav
   ```

**Colocación del archivo:**
```bash
# Estructura del proyecto
DriverDrowsinessDetector/
├── resources/
│   ├── haarcascade_eye.xml  ✓
│   └── alert.wav            ← Colocar aquí
├── src/
└── CMakeLists.txt
```

## Integración de Alertas Sonoras

### Implementación en oculox_window.cpp

La función `playAlertSound()` está integrada con el sistema de detección de somnolencia:

```cpp
// Función de reproducción de alerta sonora
void OculoxWindow::playAlertSound() {
    // Verificar si ya se está reproduciendo
    if (soundEffect && soundEffect->isPlaying()) {
        return; // No reproducir si ya está activo
    }
    
    // Verificar existencia del archivo
    QString soundPath = "qrc:/resources/alert.wav";
    if (!QFile::exists(soundPath)) {
        // Fallback: mostrar mensaje en lugar de audio
        statusLabel->setStyleSheet("background-color: red; color: white; font-weight: bold;");
        statusLabel->setText("⚠️ ALERTA: SOMNOLENCIA DETECTADA - Audio no disponible");
        return;
    }
    
    // Configurar y reproducir
    if (!soundEffect) {
        soundEffect = new QSoundEffect(this);
        soundEffect->setSource(QUrl(soundPath));
        soundEffect->setVolume(0.9);  // 90% volumen
        soundEffect->setLoopCount(1); // Una sola reproducción
    }
    
    soundEffect->play();
}

// Integración con detección EAR
void OculoxWindow::updateDrowsinessStatus(double ear, bool isDrowsy) {
    if (isDrowsy && ear < EAR_THRESHOLD) { // EAR < 0.23
        // Alerta visual
        statusLabel->setStyleSheet("background-color: red; color: white;");
        statusLabel->setText("⚠️ ALERTA: SOMNOLENCIA DETECTADA");
        
        // Alerta sonora (no bloquea UI)
        playAlertSound();
        
        // Log del evento
        qDebug() << "Drowsiness detected - EAR:" << ear << "Time:" << QTime::currentTime().toString();
    }
    else if (ear < EAR_WARNING_THRESHOLD) { // EAR entre 0.23-0.25
        statusLabel->setStyleSheet("background-color: yellow; color: black;");
        statusLabel->setText("⚠️ PRECAUCIÓN: Fatiga detectada");
    }
    else {
        statusLabel->setStyleSheet("background-color: green; color: white;");
        statusLabel->setText("✓ Estado: Alerta");
    }
}
```

### Configuración en CMakeLists.txt

Asegurar que Qt6::Multimedia esté enlazado:

```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Multimedia)
```

## Compilación

```bash
# Crear directorio de construcción
mkdir build && cd build

# Configurar con CMake
cmake ..

# Compilar
make -j$(nproc)  # Linux
# O cmake --build . --config Release  # Windows

# Ejecutar
./DriverDrowsinessDetector
```

## Uso del Sistema

1. **Inicio**: Ejecutar la aplicación
2. **Cámara**: El sistema iniciará automáticamente la captura de video
3. **Detección**: El análisis EAR comenzará inmediatamente
4. **Alertas**: 
   - **Verde**: Conductor alerta (EAR > 0.25)
   - **Amarillo**: Fatiga leve (EAR 0.23-0.25)
   - **Rojo + Audio**: Somnolencia crítica (EAR < 0.23 por >0.5s)

## Arquitectura del Proyecto

```
src/
├── camera_capture.{h,cpp}     # Captura de video
├── face_analyzer.{h,cpp}      # Detección facial
├── drowsiness_analyzer.{h,cpp} # Análisis EAR
├── detector.{h,cpp}           # Controlador principal
├── oculox_window.{h,cpp}      # Interfaz Qt6
├── utils.h                    # Utilidades
└── *_mock.h                   # Mocks para testing
```

## Fallback de Audio

Si el archivo `alert.wav` no está disponible:
- La alerta visual seguirá funcionando normalmente
- Se mostrará un mensaje adicional indicando "Audio no disponible"
- El sistema continuará operando sin interrupciones
- Se registrará un mensaje de debug en la consola

## Checklist de Verificación Final

### ✅ Recursos y Configuración
- [ ] `resources/haarcascade_eye.xml` presente y válido
- [ ] `resources/alert.wav` presente (PCM 16-bit, 44.1kHz)
- [ ] CMakeLists.txt incluye Qt6::Multimedia
- [ ] Archivos de recursos incluidos en QRC (si aplica)

### ✅ Funcionalidad de Detección
- [ ] Captura de cámara funciona correctamente
- [ ] Detección facial en tiempo real
- [ ] Cálculo EAR preciso
- [ ] Umbrales configurados (crítico: 0.23, advertencia: 0.25)
- [ ] Lógica de temporización (0.5s para activar alerta)

### ✅ Sistema de Alertas
- [ ] Alerta visual con colores (verde/amarillo/rojo)
- [ ] Alerta sonora reproduce sin bloquear UI
- [ ] Volumen configurado apropiadamente (0.9)
- [ ] No hay reproducción múltiple simultánea
- [ ] Fallback funciona si falta archivo de audio

### ✅ Interfaz de Usuario
- [ ] UI Qt6 moderna y responsiva
- [ ] Indicadores de estado claros
- [ ] Layout adaptativo
- [ ] Rendimiento fluido (>24 FPS)
- [ ] Mensajes informativos

### ✅ Arquitectura y Modularidad
- [ ] Separación clara de responsabilidades
- [ ] Clases bien encapsuladas
- [ ] Interfaces definidas entre módulos
- [ ] Código testeable con mocks disponibles
- [ ] Documentación de funciones principales

### ✅ Compilación y Portabilidad
- [ ] Compilación sin warnings en Linux
- [ ] Compilación sin warnings en Windows
- [ ] Dependencias correctamente enlazadas
- [ ] Archivos de recursos accesibles
- [ ] Ejecutable funcional generado

### ✅ Testing y Calidad
- [ ] Pruebas básicas de cada módulo
- [ ] Manejo de errores implementado
- [ ] Logging de eventos importantes
- [ ] Liberación correcta de recursos
- [ ] Sin memory leaks detectados

## Troubleshooting

### Problema: Audio no reproduce
**Solución:**
1. Verificar que `alert.wav` existe en `resources/`
2. Comprobar formato del archivo (WAV, PCM 16-bit)
3. Verificar que Qt6::Multimedia está enlazado
4. Revisar permisos de archivo

### Problema: Detección imprecisa
**Solución:**
1. Verificar iluminación adecuada
2. Ajustar umbrales EAR si es necesario
3. Calibrar cámara
4. Verificar calidad del clasificador Haar

### Problema: UI no responsiva
**Solución:**
1. Verificar que el procesamiento de video está en thread separado
2. Reducir resolución de captura si es necesario
3. Optimizar algoritmos de detección

## 🏆 Estado del Proyecto

✅ **Proyecto completamente funcional, alertas visuales y sonoras integradas, UI modernizada, recursos completos y listo para demo profesional.**

## 🔗 Enlaces Importantes

- **📥 Descargas**: [Releases](https://github.com/muracciolei/DriverDrowsinessDetector/releases)
- **🐛 Reportar Issues**: [GitHub Issues](https://github.com/muracciolei/DriverDrowsinessDetector/issues)
- **📚 Documentación**: [Wiki](https://github.com/muracciolei/DriverDrowsinessDetector/wiki)
- **💬 Discusiones**: [GitHub Discussions](https://github.com/muracciolei/DriverDrowsinessDetector/discussions)

## ⭐ ¿Te Gusta el Proyecto?

Si este proyecto te ha sido útil, ¡no olvides darle una ⭐ en GitHub!

[![GitHub stars](https://img.shields.io/github/stars/muracciolei/DriverDrowsinessDetector?style=social)](https://github.com/muracciolei/DriverDrowsinessDetector/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/muracciolei/DriverDrowsinessDetector?style=social)](https://github.com/muracciolei/DriverDrowsinessDetector/network)

---

**Versión**: 2.0.0  
**Último update**: Octubre 2025  
**Autor**: Oculox Development Team  
**Licencia**: MIT License  