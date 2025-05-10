// main.cpp
// Entry point of the application.
// Initializes GLFW, GLAD, creates a window, sets up the Renderer and Camera,
// handles user input for camera control, and runs the main game/render loop.

#include <iostream>          // For std::cout, std::cerr
#include "glad/glad.h"       // GLAD (OpenGL Loader) - Must be included before GLFW
#include <GLFW/glfw3.h>      // GLFW (Windowing and Input Library)

// Engine-specific headers (adjust paths if your structure is different)
// Assuming .h files are in include/MyFirstEngine/ and SimpleMath.h is in include/
#include "MyFirstEngine/Renderer.h"
#include "MyFirstEngine/GameObject.h" // Though GameObject itself isn't heavily used in this camera demo
#include "MyFirstEngine/Camera.h"
#include "SimpleMath.h"          // For Mat4::translate (used for model matrix)

// Window dimensions (can be modified by framebuffer_size_callback)
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// Camera setup
// Initial camera position is (0,0,3) looking towards (0,0,0) with (0,1,0) as up.
Camera camera(Vec3(0.0f, 0.0f, 3.0f));
// Variables for mouse input handling
float lastX = SCR_WIDTH / 2.0f;  // Initial mouse X position (center of screen)
float lastY = SCR_HEIGHT / 2.0f; // Initial mouse Y position (center of screen)
bool firstMouse = true;          // Flag to handle the first mouse movement smoothly

// Timing variables for frame-rate independent movement
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// A sample game object (our triangle)
GameObject triangleObject(0, "MyTriangle"); // ID 0, name "MyTriangle"

// GLFW: Callback function for window resize events
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); // Update OpenGL viewport
    SCR_WIDTH = width;               // Update global width variable for aspect ratio calculation
    SCR_HEIGHT = height;             // Update global height variable
}

// Processes keyboard input for camera movement and exiting the application
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // Close window on ESC

    // Camera movement based on deltaTime for frame-rate independence
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard("FORWARD", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard("BACKWARD", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard("LEFT", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard("RIGHT", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Move camera up
        camera.processKeyboard("UP", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // Move camera down
        camera.processKeyboard("DOWN", deltaTime);

    // Optional: If you want to move the triangle object itself with arrow keys
    // float objectMoveSpeed = 1.0f * deltaTime;
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    //     triangleObject.transform.position.y += objectMoveSpeed;
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    //     triangleObject.transform.position.y -= objectMoveSpeed;
    // if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    //     triangleObject.transform.position.x -= objectMoveSpeed;
    // if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    //     triangleObject.transform.position.x += objectMoveSpeed;
}

// GLFW: Callback function for mouse movement events
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // Prevent large jump on first mouse input
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calculate the offset movement between the last frame and current frame
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top

    lastX = xpos; // Update last mouse position
    lastY = ypos;

    // Process mouse movement to adjust camera orientation
    camera.processMouseMovement(xoffset, yoffset);
}

// GLFW: Callback function for mouse scroll wheel events
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Process scroll to adjust camera FOV (zoom)
    camera.processMouseScroll(static_cast<float>(yoffset)); // yoffset indicates vertical scroll
}


int main() {
    // --- 1. Initialize GLFW ---
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    // Configure GLFW to use OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // Required for macOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // --- 2. Create GLFW Window ---
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEngine - 3D Camera", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Make the window's context current for this thread

    // Register callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // For window resize
    glfwSetCursorPosCallback(window, mouse_callback);                 // For mouse movement
    glfwSetScrollCallback(window, scroll_callback);                   // For mouse scroll

    // Tell GLFW to capture the mouse cursor (hides it and keeps it centered)
    // This is ideal for FPS-style camera controls.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // --- 3. Initialize GLAD ---
    // GLAD loads OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // --- 4. Initialize Renderer ---
    Renderer renderer;
    if (!renderer.init()) { // Renderer::init() now enables depth testing
        std::cerr << "Failed to initialize renderer" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set initial position for our triangle object (if you want it somewhere other than origin)
    // For this demo, the triangle will be at the world origin.
    triangleObject.transform.position = Vec3(0.0f, 0.0f, 0.0f);
    // You can place multiple objects by creating more GameObject instances
    // and drawing them with different model matrices.

    // --- 5. Main Game Loop ---
    while (!glfwWindowShouldClose(window)) {
        // --- a. Calculate DeltaTime ---
        // Time logic for frame-rate independent movement and animations
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- b. Process Input ---
        processInput(window);

        // --- c. Update Game Logic ---
        // (e.g., physics, animations, AI - not implemented in this simple demo)

        // --- d. Render ---
        // Create model matrix for the triangle object.
        // This matrix transforms the triangle from its local space to world space.
        // For now, it only applies the translation from the object's transform.
        Mat4 modelMatrix = Mat4::translate(triangleObject.transform.position);
        // To add rotation and scale:
        // Mat4 scaleMatrix = Mat4::scale(triangleObject.transform.scale);
        // Mat4 rotationX = Mat4::rotateX(triangleObject.transform.rotation.x); // Assuming rotate functions in Mat4
        // Mat4 rotationY = Mat4::rotateY(triangleObject.transform.rotation.y);
        // Mat4 rotationZ = Mat4::rotateZ(triangleObject.transform.rotation.z);
        // modelMatrix = Mat4::translate(triangleObject.transform.position) * rotationZ * rotationY * rotationX * scaleMatrix;


        // Get view and projection matrices from the camera
        Mat4 viewMatrix = camera.getViewMatrix();
        // Calculate aspect ratio based on current window size
        float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
        if (SCR_HEIGHT == 0) aspectRatio = 1.0f; // Avoid division by zero if window is minimized
        Mat4 projectionMatrix = camera.getProjectionMatrix(aspectRatio);

        // Call the renderer to draw the scene (our triangle)
        renderer.draw(modelMatrix, viewMatrix, projectionMatrix);

        // --- e. Swap Buffers and Poll Events ---
        glfwSwapBuffers(window); // Swaps the front and back buffers (double buffering)
        glfwPollEvents();        // Checks for and processes events (keyboard, mouse, window, etc.)
    }

    // --- 6. Cleanup ---
    // Renderer's destructor will clean up its resources (VAO, VBO, shader).
    // Camera and GameObject are stack-allocated and will be cleaned up automatically.
    glfwTerminate(); // Terminate GLFW, cleaning up all its resources
    return 0;
}
