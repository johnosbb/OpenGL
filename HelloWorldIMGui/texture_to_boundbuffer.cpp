#include <iostream>
#include <opencv2/opencv.hpp>

#include <GL/gl3w.h> // OpenGL function loader
#include <GLFW/glfw3.h>

// OpenGL error checking
#define GL_CHECK(x)                                                                       \
    do                                                                                    \
    {                                                                                     \
        x;                                                                                \
        GLenum err = glGetError();                                                        \
        if (err != GL_NO_ERROR)                                                           \
        {                                                                                 \
            std::cerr << "OpenGL error at line " << __LINE__ << ": " << err << std::endl; \
        }                                                                                 \
    } while (0)

// Constants
const int IMAGE_WIDTH = 512;
const int IMAGE_HEIGHT = 512;

// Global variables
GLuint textureID;
GLuint framebuffer;
GLuint renderbuffer;

// Function to create a blue OpenCV Mat and convert it to an OpenGL texture
GLuint createTextureFromMat()
{
    // Create a blue image (RGBA format)
    cv::Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC4, cv::Scalar(0, 0, 255, 255)); // RGBA format (Blue)

    // Generate OpenGL texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload image data to the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

// Function to set up framebuffer and attach texture
void setupFramebuffer(GLuint texture)
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Create a renderbuffer for depth and stencil (optional, but needed for completeness)
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, IMAGE_WIDTH, IMAGE_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Main rendering function
void render()
{
    // Bind framebuffer for reading
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer for drawing

    // Blit (copy) framebuffer to default framebuffer
    glBlitFramebuffer(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Set OpenGL version to 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *window = glfwCreateWindow(IMAGE_WIDTH, IMAGE_HEIGHT, "OpenGL Blit Example", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize OpenGL loader (gl3w)
    if (gl3wInit() != 0)
    {
        std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
        return -1;
    }

    // Create texture from OpenCV Mat
    textureID = createTextureFromMat();

    // Setup framebuffer
    setupFramebuffer(textureID);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render framebuffer to screen
        render();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteTextures(1, &textureID);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
    glfwTerminate();

    return 0;
}
