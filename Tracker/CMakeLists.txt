cmake_minimum_required(VERSION 3.10)
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 11)

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)  # Add OpenCV

set(IMGUI_SRC
    src/imgui.cpp
    src/imgui_draw.cpp
    src/imgui_widgets.cpp
    src/imgui_tables.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
)

# # Enable AddressSanitizer if supported
# if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#     set(SANITIZER_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
# endif()


set(GL3W_SRC src/gl3w.c)

add_library(imgui ${IMGUI_SRC})
add_library(gl3w ${GL3W_SRC})

target_include_directories(imgui PUBLIC include)
target_include_directories(gl3w PUBLIC include)

# Add OpenCV include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Link with executable
add_executable(track main.cpp)




# Link executables with required libraries, including OpenCV
target_link_libraries(track imgui gl3w glfw OpenGL::GL ${OpenCV_LIBS})


# Ensure AddressSanitizer is linked
# if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#     target_link_libraries(track -fsanitize=address)
# endif()