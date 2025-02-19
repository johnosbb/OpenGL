cmake_minimum_required(VERSION 3.10)
project(OpenGL_HelloWorld)

set(CMAKE_CXX_STANDARD 11)


# Set OpenGL to prefer GLVND (modern implementation)
cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)

# Find OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Add gl3w (instead of glad)
add_library(gl3w src/gl3w.c)
target_include_directories(gl3w PUBLIC include)

# Create executable
add_executable(hello_opengl main_glw3.cpp)
target_link_libraries(hello_opengl PRIVATE gl3w glfw OpenGL::GL)
