# Script para configurar y compilar con vcpkg y OpenCV

# Configurar vcpkg toolchain
$vcpkgRoot = "C:\vcpkg"
$vcpkgToolchain = "$vcpkgRoot\scripts\buildsystems\vcpkg.cmake"

Write-Host "Configurando proyecto con vcpkg y OpenCV..." -ForegroundColor Green

# Verificar que vcpkg existe
if (-Not (Test-Path $vcpkgRoot)) {
    Write-Host "Error: vcpkg no encontrado en $vcpkgRoot" -ForegroundColor Red
    Write-Host "Por favor, clona vcpkg primero:" -ForegroundColor Yellow
    Write-Host "git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg" -ForegroundColor Yellow
    Write-Host "cd C:\vcpkg && .\bootstrap-vcpkg.bat" -ForegroundColor Yellow
    exit 1
}

# Verificar que OpenCV está instalado
$opencvInstalled = & "$vcpkgRoot\vcpkg.exe" list | Select-String "opencv"
if (-Not $opencvInstalled) {
    Write-Host "OpenCV no está instalado. Instalando..." -ForegroundColor Yellow
    & "$vcpkgRoot\vcpkg.exe" install opencv:x64-windows
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error instalando OpenCV" -ForegroundColor Red
        exit 1
    }
}

Write-Host "OpenCV disponible en vcpkg" -ForegroundColor Green

# Configurar build directory
$buildDir = "build-vcpkg"
if (Test-Path $buildDir) {
    Remove-Item $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Configurar CMake con vcpkg
Write-Host "Configurando CMake con vcpkg..." -ForegroundColor Cyan
Set-Location $buildDir

$cmakeCommand = @(
    "cmake"
    ".."
    "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain"
    "-DOPENCV_MOCK_BUILD=OFF"
    "-DCMAKE_BUILD_TYPE=Release"
)

Write-Host "Ejecutando: $($cmakeCommand -join ' ')" -ForegroundColor Gray
& $cmakeCommand[0] $cmakeCommand[1..($cmakeCommand.Length-1)]

if ($LASTEXITCODE -eq 0) {
    Write-Host "Configuracion exitosa" -ForegroundColor Green
    
    # Compilar
    Write-Host "Compilando..." -ForegroundColor Cyan
    cmake --build . --config Release
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Compilacion exitosa con OpenCV real!" -ForegroundColor Green
        Write-Host "Ejecutable en: .\Release\Oculox_DrowsinessDetector_StatusBar.exe" -ForegroundColor Yellow
    } else {
        Write-Host "Error en la compilacion" -ForegroundColor Red
    }
} else {
    Write-Host "Error en la configuracion de CMake" -ForegroundColor Red
}

Set-Location ..