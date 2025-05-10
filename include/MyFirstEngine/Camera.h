// Camera.h
// Defines a basic 3D camera capable of movement and orientation.
// It generates View and Projection matrices for rendering.

#ifndef CAMERA_H
#define CAMERA_H

#include "../SimpleMath.h" // Path relative to include/MyFirstEngine/Camera.h
                           // This includes Vec3 and Mat4 definitions.

// Forward declaration of GLFWwindow to avoid including glfw3.h in this header.
// This is a good practice to reduce compile times and dependencies in headers.
// The actual GLFWwindow pointer will be used in main.cpp.
struct GLFWwindow;

class Camera {
public:
    // Camera Attributes
    Vec3 position;  // The camera's position in world space
    Vec3 front;     // The direction the camera is looking (normalized)
    Vec3 up;        // The camera's local up vector (calculated)
    Vec3 right;     // The camera's local right vector (calculated)
    Vec3 worldUp;   // A fixed up vector for the world (e.g., positive Y-axis)

    // Euler Angles for orientation
    float yaw;      // Rotation around the Y-axis
    float pitch;    // Rotation around the X-axis

    // Camera options
    float movementSpeed;    // Speed of camera movement
    float mouseSensitivity; // How sensitive mouse movement is for looking around
    float fov;              // Field of View in degrees (for perspective projection)

    // Constructor with initial values
    // Yaw is typically initialized to -90.0 degrees to look along the negative Z-axis.
    // Pitch is typically initialized to 0.0 degrees (looking straight ahead).
    Camera(Vec3 position = Vec3(0.0f, 0.0f, 3.0f), // Default position
           Vec3 up = Vec3(0.0f, 1.0f, 0.0f),      // Default world up vector
           float yaw = -90.0f,
           float pitch = 0.0f);

    // Returns the view matrix calculated using the camera's current position and orientation
    Mat4 getViewMatrix();

    // Returns the projection matrix based on FOV, aspect ratio, and near/far planes
    Mat4 getProjectionMatrix(float aspectRatio); // Aspect ratio (width / height)

    // Processes keyboard input for camera movement
    // 'direction' can be "FORWARD", "BACKWARD", "LEFT", "RIGHT", "UP", "DOWN"
    // 'deltaTime' is the time elapsed since the last frame, for frame-rate independent movement
    void processKeyboard(const char* direction, float deltaTime);

    // Processes mouse movement input for camera orientation
    // 'xoffset' and 'yoffset' are the changes in mouse position since the last frame
    // 'constrainPitch' prevents the camera from flipping upside down
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Processes mouse scroll wheel input, typically for zooming (adjusting FOV)
    // 'yoffset' is the amount the scroll wheel has moved
    void processMouseScroll(float yoffset);

private:
    // Recalculates the 'front', 'right', and 'up' vectors based on the current 'yaw' and 'pitch'
    void updateCameraVectors();
};

#endif // CAMERA_H
