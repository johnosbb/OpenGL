#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <algorithm> // Needed for std::clamp

// OpenGL Texture
GLuint g_imageTexture = 0;
int image_width = 0, image_height = 0;
unsigned char *loaded_image_data = nullptr;

float zoom_factor = 1.0f;               // Default zoom level
ImVec2 pan_offset = ImVec2(0.0f, 0.0f); // Used for panning

// Selection state
bool selecting = false;
ImVec2 selection_start(0, 0);
ImVec2 selection_end(0, 0);

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

bool LoadTextureFromFileAndFitToWindow(const char *filename, GLFWwindow *window)
{
    int width, height, channels;
    loaded_image_data = stbi_load(filename, &image_width, &image_height, &channels, 4);
    if (!loaded_image_data)
        return false;

    // Get actual window size
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    glGenTextures(1, &g_imageTexture);
    glBindTexture(GL_TEXTURE_2D, g_imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_image_data);

    // Store original image size
    image_width = image_width;
    image_height = image_height;

    // // Get actual window size
    // int window_width, window_height;
    // glfwGetFramebufferSize(window, &window_width, &window_height);

    // Compute scaling factors to fit the image in the window
    float scale_x = (float)window_width / image_width;
    float scale_y = (float)window_height / image_height;

    // Use the smaller scale factor to maintain aspect ratio
    zoom_factor = std::min(scale_x, scale_y);

    return true;
}

void SaveSelectionAsImage(const std::string &filename, ImVec2 start, ImVec2 end, ImVec2 image_pos)
{
    if (!loaded_image_data)
        return;

    // Correct for the image offset inside the ImGui window
    int x1 = std::round(start.x - image_pos.x);
    int y1 = std::round(start.y - image_pos.y);
    int x2 = std::round(end.x - image_pos.x);
    int y2 = std::round(end.y - image_pos.y);

    // Ensure selection stays within image bounds
    x1 = std::max(0, std::min(x1, image_width));
    y1 = std::max(0, std::min(y1, image_height));
    x2 = std::max(0, std::min(x2, image_width));
    y2 = std::max(0, std::min(y2, image_height));

    int sel_width = x2 - x1;
    int sel_height = y2 - y1;

    // Prevent invalid selections
    if (sel_width <= 0 || sel_height <= 0)
        return;

    // Copy selected pixels
    std::vector<unsigned char> selected_region(sel_width * sel_height * 4);
    for (int y = 0; y < sel_height; y++)
    {
        memcpy(
            &selected_region[y * sel_width * 4],                   // Destination row
            &loaded_image_data[((y1 + y) * image_width + x1) * 4], // Source row
            sel_width * 4                                          // Bytes per row (RGBA)
        );
    }

    // Save as PNG
    stbi_write_png(filename.c_str(), sel_width, sel_height, 4, selected_region.data(), sel_width * 4);
    std::cout << "Selection saved as " << filename << std::endl;
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "ImGui Image Selection", nullptr, nullptr);
    if (!window)
        return -1;

    glfwMakeContextCurrent(window);
    gl3wInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!LoadTextureFromFile("test.png"))
        return -1;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Get actual window size
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);

        // Compute scale factor to fit image inside the window while maintaining aspect ratio
        float scale_x = (float)window_width / image_width;
        float scale_y = (float)window_height / image_height;
        float scale = std::min(scale_x, scale_y); // Maintain aspect ratio

        // Calculate the new scaled image size
        ImVec2 image_size(image_width * scale, image_height * scale);

        // Center the image inside the window
        ImVec2 window_center(window_width * 0.5f, window_height * 0.5f);
        ImVec2 image_pos(window_center.x - image_size.x * 0.5f, window_center.y - image_size.y * 0.5f);

        ImGui::SetNextWindowPos(ImVec2(0, 0));                // This sets the top-left corner of the window to (0,0), meaning it will start at the top-left of the main application window.
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize); // GetIO().DisplaySize returns the current size of the main application window (the GLFW window in this case). SetNextWindowSize instructs ImGui that the next window created should fill the entire application window
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        // Move the cursor to where the image should be drawn
        ImGui::SetCursorScreenPos(image_pos);                            // determining the top-left corner of the image
        ImGui::Image((ImTextureID)(intptr_t)g_imageTexture, image_size); // This renders the image inside the ImGui window.

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, window_width, window_height); // It sets the viewport, which is the rectangular area of the window where OpenGL will map its coordinate system.
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

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
