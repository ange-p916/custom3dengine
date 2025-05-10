// main.cpp
// Basic game loop, input handling, and moving a GameObject.

#include <iostream>
#include "glad/glad.h" 
#include <GLFW/glfw3.h>

#include "MyFirstEngine/GameObject.h" 
#include "MyFirstEngine/Transform.h"
#include "MyFirstEngine/Renderer.h"
// etc.

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Our single game object
GameObject triangleObject(0, "MyTriangle"); // ID 0, name "MyTriangle"
float moveSpeed = 0.01f; // Speed at which the triangle moves

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Updated input processing
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Movement controls for the triangleObject
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        triangleObject.transform.position.y += moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        triangleObject.transform.position.y -= moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        triangleObject.transform.position.x -= moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        triangleObject.transform.position.x += moveSpeed;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEngine - Movable Triangle", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    Renderer renderer;
    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        glfwTerminate();
        return -1;
    }

    // --- Game Loop ---
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Update (game logic would go here - for now, input directly changes position)
        // triangleObject.transform.position.x += 0.0001f; // Example of continuous movement

        // Render
        // Pass the current position of our triangleObject to the renderer
        renderer.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate(); 
    return 0;
}
