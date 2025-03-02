cmake_minimum_required(VERSION 3.10)
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 11)

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)  
find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)  # Find X11
pkg_check_modules(TESSERACT REQUIRED tesseract)

include_directories(${TESSERACT_INCLUDE_DIRS})
link_directories(${TESSERACT_LIBRARY_DIRS})

# Ensure that OpenCV and Tesseract include directories are available
include_directories(${OpenCV_INCLUDE_DIRS} ${Tesseract_INCLUDE_DIRS})

set(IMGUI_SRC
    src/imgui.cpp
    src/imgui_draw.cpp
    src/imgui_widgets.cpp
    src/imgui_tables.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
)

set(GL3W_SRC src/gl3w.c)

add_library(imgui ${IMGUI_SRC})
add_library(gl3w ${GL3W_SRC})

target_include_directories(imgui PUBLIC include)
target_include_directories(gl3w PUBLIC include)

# Define executables
add_executable(gesture gesture.cpp)
add_executable(face_recognition face_recognition.cpp)
add_executable(face_recognition_training face_recognition_training.cpp)
add_executable(take_photo take_photo.cpp)

# Link each executable with OpenCV, Tesseract, and necessary libraries
target_link_libraries(gesture imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(face_recognition imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(face_recognition_training imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(take_photo imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})