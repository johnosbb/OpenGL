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

// Function to load the image as a texture
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

// Resize GLFW window to match image dimensions
void ResizeGLFWWindow(GLFWwindow *window, int width, int height)
{
    glfwSetWindowSize(window, width, height);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
        return -1;

    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create initial GLFW window
    GLFWwindow *window = glfwCreateWindow(800, 600, "ImGui Image Viewer", nullptr, nullptr);
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

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set up ImGui window to match GLFW window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        // Render image at its original size
        ImGui::Image((ImTextureID)(intptr_t)g_imageTexture, ImVec2((float)image_width, (float)image_height));

        ImGui::End();
        ImGui::Render();

        // Adjust OpenGL viewport
        int window_width, window_height;
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
