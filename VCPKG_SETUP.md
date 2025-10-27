# 🛠️ CONFIGURACIÓN PERMANENTE DE VCPKG

## ✅ vcpkg YA ESTÁ INSTALADO EN C:\vcpkg

### 🔧 CONFIGURAR PATH PERMANENTEMENTE

#### Opción 1: A través de la Interfaz Gráfica (Recomendado)
1. **Abrir Variables de Entorno**:
   - Presiona `Win + R`, escribe `sysdm.cpl` y presiona Enter
   - Ve a la pestaña "Opciones Avanzadas"
   - Haz clic en "Variables de entorno..."

2. **Agregar a PATH del Sistema**:
   - En "Variables del sistema", selecciona "Path" y haz clic en "Editar..."
   - Haz clic en "Nuevo" y agrega: `C:\vcpkg`
   - Haz clic en "Aceptar" en todas las ventanas

3. **Verificar**:
   - Abre una nueva ventana de PowerShell
   - Ejecuta: `vcpkg version`

#### Opción 2: PowerShell como Administrador
```powershell
# Ejecutar PowerShell como Administrador
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\vcpkg", [EnvironmentVariableTarget]::Machine)
```

### 🎯 USAR VCPKG CON NUESTRO PROYECTO

#### Instalar OpenCV (ya hecho):
```bash
vcpkg install opencv:x64-windows
```

#### Compilar con OpenCV real:
```bash
# Ejecutar el script automatizado
.\build-with-vcpkg.ps1

# O manualmente:
mkdir build-vcpkg
cd build-vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DOPENCV_MOCK_BUILD=OFF
cmake --build . --config Release
```

### 📦 PAQUETES ÚTILES PARA FUTUROS PROYECTOS

```bash
# Computer Vision
vcpkg install opencv:x64-windows
vcpkg install opencv[contrib]:x64-windows  # Módulos extras
vcpkg install dlib:x64-windows             # Machine Learning

# GUI/Graphics
vcpkg install qt5:x64-windows              # Qt5 GUI framework
vcpkg install sfml:x64-windows             # Simple graphics
vcpkg install glfw3:x64-windows            # OpenGL window management

# Utilities
vcpkg install boost:x64-windows            # Boost libraries
vcpkg install fmt:x64-windows              # Modern C++ formatting
vcpkg install spdlog:x64-windows           # Fast logging library
vcpkg install json:x64-windows             # JSON parsing

# Math/Science
vcpkg install eigen3:x64-windows           # Linear algebra
vcpkg install ceres:x64-windows            # Non-linear optimization
```

### 🚀 VENTAJAS DE TENER VCPKG EN PATH

✅ **Comando global**: `vcpkg` disponible desde cualquier directorio
✅ **Fácil instalación**: `vcpkg install <package>:x64-windows`
✅ **Gestión simple**: `vcpkg list`, `vcpkg search <term>`
✅ **Actualizaciones**: `vcpkg upgrade`
✅ **Integración CMake**: Toolchain automático disponible

### 📋 COMANDOS ÚTILES DE VCPKG

```bash
# Buscar paquetes
vcpkg search opencv
vcpkg search qt

# Ver paquetes instalados
vcpkg list

# Información de un paquete
vcpkg show opencv

# Actualizar todos los paquetes
vcpkg upgrade

# Remover un paquete
vcpkg remove opencv:x64-windows

# Integrar con Visual Studio
vcpkg integrate install
```

---

## 🎉 ESTADO ACTUAL

✅ **vcpkg configurado** en C:\vcpkg
✅ **OpenCV instalado** vía vcpkg  
✅ **Script automatizado** creado (`build-with-vcpkg.ps1`)
✅ **Proyecto listo** para compilar con cámara real

**¡Solo falta agregar C:\vcpkg al PATH permanentemente y tendrás vcpkg disponible globalmente!**