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
add_executable(helloworld main.cpp)
add_executable(helloworld_blit blit_example.cpp)
add_executable(helloworld_shader shader_example.cpp)
add_executable(helloworld_mat mat_to_texture.cpp)
add_executable(helloworld_bound texture_to_boundbuffer.cpp)
add_executable(read_number_plate read_number_plate.cpp)
add_executable(read_number_plate_static read_number_plate_static.cpp)
add_executable(read_number_plate_roi read_number_plate_roi.cpp)
add_executable(face_recognition face_recognition.cpp)
add_executable(imgui_display imgui_display.cpp)
add_executable(imgui_select_region imgui_select_region.cpp)
add_executable(imgui_with_zoom imgui_with_zoom.cpp)
add_executable(imgui_fit_window_to_image imgui_fit_window_to_image.cpp)
add_executable(imgui_fscale_image imgui_fit_window_to_image.cpp)

# Link each executable with OpenCV, Tesseract, and necessary libraries
target_link_libraries(helloworld imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(helloworld_blit imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(helloworld_shader imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(helloworld_mat imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(helloworld_bound imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})
target_link_libraries(read_number_plate imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})
target_link_libraries(read_number_plate_static imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})
target_link_libraries(read_number_plate_roi imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})
target_link_libraries(face_recognition imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES} X11)
target_link_libraries(imgui_display imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})
target_link_libraries(imgui_select_region imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})
target_link_libraries(imgui_with_zoom imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})
target_link_libraries(imgui_fit_window_to_image imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})
target_link_libraries(imgui_fscale_image imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS} ${TESSERACT_LIBRARIES})