// main.cpp
// Entry point for the basic 3D renderer.
// Initializes GLFW, GLAD, creates a window, and runs the main render loop.

#include <iostream>
#include "glad/glad.h" // GLAD manages OpenGL function pointers - Make sure to include GLAD before GLFW
#include <GLFW/glfw3.h> // GLFW for windowing and input

#include "Renderer.h" // Make sure you have Renderer.h and Renderer.cpp
#include "Shader.h"   // Make sure you have Shader.h and Shader.cpp

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Callback function for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // --- 1. Initialize GLFW ---
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile (modern OpenGL)
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS
#endif

    // --- 2. Create a GLFW window ---
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEngine - Triangle", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Register the framebuffer size callback to adjust viewport on window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- 3. Initialize GLAD ---
    // GLAD loads all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate(); // Terminate GLFW before exiting
        return -1;
    }

    // --- 4. Setup Renderer ---
    // Ensure Renderer.h and Renderer.cpp are in your project and compiled.
    Renderer renderer;
    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        glfwTerminate();
        return -1;
    }

    // --- 5. Main Render Loop ---
    while (!glfwWindowShouldClose(window)) {
        // Input processing
        processInput(window);

        // Rendering commands here
        renderer.draw();

        // Swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- 6. Cleanup ---
    // Renderer cleans up its own resources in its destructor (implicitly called when 'renderer' goes out of scope)
    // Shader program is cleaned up by the Renderer's destructor.
    glfwTerminate(); // Terminate GLFW, clearing all previously allocated GLFW resources.
    return 0;
}
