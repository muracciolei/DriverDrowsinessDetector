cmake_minimum_required(VERSION 3.16)
project(Oculox_DrowsinessDetector_StatusBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Option to build with mock OpenCV (for systems without OpenCV)
option(OPENCV_MOCK_BUILD "Build with mock OpenCV implementation" OFF)

# Set Qt6 path if not using vcpkg
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
    set(Qt6_DIR "C:/Qt/6.9.3/msvc2022_64/lib/cmake/Qt6")
endif()

# Find Qt6
find_package(Qt6 COMPONENTS Widgets Multimedia REQUIRED)

# Try to find OpenCV if not in mock mode
if(NOT OPENCV_MOCK_BUILD)
    find_package(OpenCV QUIET)
    if(NOT OpenCV_FOUND)
        message(WARNING "OpenCV not found. Falling back to mock implementation.")
        set(OPENCV_MOCK_BUILD ON)
    else()
        message(STATUS "Found OpenCV ${OpenCV_VERSION}")
        message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
        message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    endif()
endif()

# Compiler warnings settings
if(MSVC)
    add_compile_options(/W4 /WX-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Set mock build definition if needed
if(OPENCV_MOCK_BUILD)
    add_compile_definitions(OPENCV_MOCK_BUILD)
    message(STATUS "Building with mock OpenCV implementation")
else()
    message(STATUS "Building with real OpenCV implementation")
endif()

add_executable(Oculox_DrowsinessDetector_StatusBar
    main.cpp
    src/oculox_window.cpp
    src/oculox_window.h
    src/detector.cpp
    src/detector.h
)

# Link libraries
if(OPENCV_MOCK_BUILD)
    target_link_libraries(Oculox_DrowsinessDetector_StatusBar
        Qt6::Widgets
        Qt6::Multimedia
    )
else()
    target_link_libraries(Oculox_DrowsinessDetector_StatusBar
        Qt6::Widgets
        Qt6::Multimedia
        ${OpenCV_LIBS}
    )
    target_include_directories(Oculox_DrowsinessDetector_StatusBar PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()