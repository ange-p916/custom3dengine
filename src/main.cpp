// main.cpp
// Basic game loop, input handling, and moving a GameObject.
// WITH DEBUGGING FOR MOVEMENT

#include <iostream> // For std::cout
#include "glad/glad.h" 
#include <GLFW/glfw3.h> 

// Assuming your headers are in include/MyFirstEngine/
// Adjust if they are directly in include/ or elsewhere
#include "MyFirstEngine/Renderer.h" 
#include "MyFirstEngine/GameObject.h" 

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Our single game object
GameObject triangleObject(0, "MyTriangle"); 
float moveSpeed = 0.01f; // Speed at which the triangle moves (ensure this is not 0.0f)

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool moved = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        triangleObject.transform.position.y += moveSpeed;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        triangleObject.transform.position.y -= moveSpeed;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        triangleObject.transform.position.x -= moveSpeed;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        triangleObject.transform.position.x += moveSpeed;
        moved = true;
    }

    if (moved) {
        // Print the position if a movement key was pressed
        std::cout << "Triangle Position: X=" << triangleObject.transform.position.x
                  << ", Y=" << triangleObject.transform.position.y
                  << ", Z=" << triangleObject.transform.position.z << std::endl;
    }
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

    // Assuming Renderer.h is in include/MyFirstEngine/
    // Adjust include path if necessary
    Renderer renderer; 
    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Initial Triangle Position: X=" << triangleObject.transform.position.x
              << ", Y=" << triangleObject.transform.position.y
              << ", Z=" << triangleObject.transform.position.z << std::endl;
    std::cout << "Move Speed: " << moveSpeed << std::endl;
    std::cout << "Use W, A, S, D keys to move the triangle. ESC to close." << std::endl;


    // --- Game Loop ---
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Update (game logic - position is updated in processInput for now)

        // Render
        renderer.draw(triangleObject.transform.position);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate(); 
    return 0;
}
