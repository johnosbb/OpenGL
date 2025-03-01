#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// OpenGL Texture
GLuint g_imageTexture = 0;
int image_width = 0, image_height = 0;

// Check for OpenGL errors
void CheckGLErrors(const std::string &label)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error (" << label << "): " << err << std::endl;
    }
}

// Load an Image into OpenGL Texture
bool LoadTextureFromFile(const char *filename)
{
    int width, height, channels;
    unsigned char *image_data = stbi_load(filename, &width, &height, &channels, 4);
    if (!image_data)
    {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return false;
    }

    glGenTextures(1, &g_imageTexture);
    glBindTexture(GL_TEXTURE_2D, g_imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    CheckGLErrors("Texture Creation");

    if (g_imageTexture == 0)
    {
        std::cout << "Error: Image texture was not generated!" << std::endl;
    }
    else
    {
        std::cout << "Generated texture ID: " << g_imageTexture << std::endl;
    }

    stbi_image_free(image_data);
    image_width = width;
    image_height = height;
    return true;
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Create OpenGL Window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "ImGui OpenGL Example", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gl3wInit();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Load Image
    if (!LoadTextureFromFile("test.png"))
    {
        std::cerr << "Could not load image!" << std::endl;
        return -1;
    }

    // UI State
    bool show_demo_window = false;
    int dropdown_selection = 0;

    // Main Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI Controls
        ImGui::Begin("Control Panel");
        if (ImGui::Button("Example Button"))
        {
            std::cout << "Button Clicked!" << std::endl;
        }
        ImGui::Combo("Dropdown", &dropdown_selection, "Option 1\0Option 2\0Option 3\0\0");
        ImGui::End();

        // Image Display
        ImGui::Begin("Image Viewer"); // The ImGui render begings with ImGui::Begin().
        // Get screen position of image
        ImVec2 image_pos = ImGui::GetCursorScreenPos();
        ImVec2 imageDisplaySize = ImVec2((float)image_width, (float)image_height);

        if (g_imageTexture)
        {
            ImGui::Image(static_cast<ImTextureID>(g_imageTexture), imageDisplaySize); // Tells ImGui to render an image using the provided OpenGL texture (g_imageTexture) inside the ImGui window.
            // Draw overlay rectangle
            ImDrawList *draw_list = ImGui::GetWindowDrawList();
            if (1) // Draw a red outline rectangle at a particular location on the screen
            {
                draw_list->AddRect(
                    image_pos,
                    ImVec2(image_pos.x + image_width, image_pos.y + image_height),
                    IM_COL32(255, 0, 0, 255),
                    0.0f,
                    0,
                    3.0f);
            }
            else
            {
                // Draw a semi-opaque rectangle at a particular location on the screen
                draw_list->AddRectFilled(
                    ImVec2(image_pos.x + 50, image_pos.y + 50),   // Top-left
                    ImVec2(image_pos.x + 200, image_pos.y + 150), // Bottom-right
                    IM_COL32(0, 255, 0, 128)                      // Semi-transparent green
                );
            }
        }
        else
        {
            ImGui::Text("No image loaded.");
        }

        ImGui::End(); // This closes the last ImGui window that was opened with ImGui::Begin().

        // Render ImGui
        ImGui::Render(); // This finalizes the ImGui frame. It tells ImGui that all UI elements have been added and are ready to be processed.  It also computes the draw list (commands and vertex buffers) needed to render the UI.

        glViewport(0, 0, 800, 600);
        glClear(GL_COLOR_BUFFER_BIT); // This clears the screen before drawing.
        // This renders ImGui's UI elements using OpenGL.
        // ImGui::GetDrawData() fetches the draw commands that ImGui prepared in ImGui::Render().
        // ImGui_ImplOpenGL3_RenderDrawData(...) then executes these OpenGL draw calls to display the UI.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Since we are using double-buffering, this swaps the front and back buffers.
        // OpenGL draws to a hidden (back) buffer while the visible (front) buffer is displayed.
        // glfwSwapBuffers() swaps them, making the newly drawn frame visible.
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &g_imageTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
