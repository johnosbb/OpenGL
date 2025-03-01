#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// OpenGL Texture
GLuint g_imageTexture = 0;
int image_width = 0, image_height = 0;
unsigned char *loaded_image_data = nullptr;

float zoom = 1.0f, min_zoom = 1.0f; // Zoom and min zoom factor
int window_width = 800, window_height = 600;
ImVec2 panOffset(0, 0); // Panning offset

// Load image as OpenGL texture
bool LoadTextureFromFile(const char *filename)
{
    int width, height, channels;
    loaded_image_data = stbi_load(filename, &width, &height, &channels, 4);
    if (!loaded_image_data)
        return false;

    glGenTextures(1, &g_imageTexture);
    glBindTexture(GL_TEXTURE_2D, g_imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_image_data);

    image_width = width;
    image_height = height;
    return true;
}

// Resize GLFW window and update min_zoom
void ResizeGLFWWindow(GLFWwindow *window, int width, int height)
{
    window_width = width;
    window_height = height;
    glfwSetWindowSize(window, width, height);

    min_zoom = std::max((float)window_width / image_width, (float)window_height / image_height);
    zoom = std::max(zoom, min_zoom);
    panOffset = ImVec2(0, 0); // Reset pan when resizing
}

// Process input for zooming & panning
void ProcessInput(GLFWwindow *window, ImVec2 scaledSize)
{
    float panSpeed = 20.0f; // Pixels per arrow key press

    // // Zoom In/Out
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
    //     zoom = std::min(zoom * 1.1f, 5.0f);
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
    //     zoom = std::max(zoom * 0.9f, min_zoom);

     // Ensure panning only works when zoomed in
    if (zoom > min_zoom)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            panOffset.x = std::min(panOffset.x + panSpeed, (scaledSize.x - window_width) / 2);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            panOffset.x = std::max(panOffset.x - panSpeed, -(scaledSize.x - window_width) / 2);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            panOffset.y = std::min(panOffset.y + panSpeed, (scaledSize.y - window_height) / 2);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            panOffset.y = std::max(panOffset.y - panSpeed, -(scaledSize.y - window_height) / 2);
    }
}

// Mouse scroll callback for zooming
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        zoom = std::min(zoom * 1.1f, 5.0f);
    else if (yoffset < 0)
        zoom = std::max(zoom * 0.9f, min_zoom);
}

// GLFW framebuffer resize callback
void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    ResizeGLFWWindow(window, width, height);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "ImGui Image Viewer", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
    gl3wInit();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Load image and resize window
    if (LoadTextureFromFile("test.png"))
    {
        ResizeGLFWWindow(window, image_width, image_height);
    }

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Compute scaled size of image
        ImVec2 scaledSize = ImVec2(image_width * zoom, image_height * zoom);
        ProcessInput(window, scaledSize);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set ImGui window size to match GLFW window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        // Center the image & apply panning
        ImVec2 imagePos = ImVec2((window_width - scaledSize.x) / 2 + panOffset.x,
                                 (window_height - scaledSize.y) / 2 + panOffset.y);
        ImGui::SetCursorScreenPos(imagePos);
        ImGui::Image((ImTextureID)(intptr_t)g_imageTexture, scaledSize);

        ImGui::End();
        ImGui::Render();

        // Adjust OpenGL viewport
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &g_imageTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    if (loaded_image_data)
        stbi_image_free(loaded_image_data);

    return 0;
}
