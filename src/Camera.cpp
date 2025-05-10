// Camera.cpp
// Implements the 3D Camera class, handling its logic for movement,
// orientation, and matrix generation.

#include "MyFirstEngine/Camera.h" // Path to Camera.h, assuming Camera.h is in include/MyFirstEngine/
#include <cmath>                  // For M_PI (or define your own PI), sin, cos, tan
#include <algorithm>              // For std::min, std::max (used for constraining pitch and FOV)
#include <cstring>                // For strcmp (used in processKeyboard)
#include <iostream>               // For potential debugging output (optional)

// Define M_PI if it's not available (common on Windows with MSVC before C++20)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor: Initializes camera attributes
Camera::Camera(Vec3 position, Vec3 up, float yaw, float pitch)
    : front(Vec3(0.0f, 0.0f, -1.0f)), // Initial 'front' vector points down the negative Z-axis
      movementSpeed(2.5f),           // Default movement speed
      mouseSensitivity(0.1f),        // Default mouse sensitivity
      fov(45.0f) {                   // Default Field of View (in degrees)
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors(); // Calculate initial front, right, and up vectors
}

// Calculates and returns the view matrix using the Mat4::lookAt function
Mat4 Camera::getViewMatrix() {
    // The lookAt function takes the camera's position, the point it's looking at (position + front),
    // and the world's up vector to construct the view matrix.
    // We use worldUp here instead of the camera's local 'up' to prevent potential roll issues
    // when the camera is pitched straight up or down, though our updateCameraVectors recalculates 'up'.
    return Mat4::lookAt(position, position + front, worldUp);
}

// Calculates and returns the projection matrix using Mat4::perspective
Mat4 Camera::getProjectionMatrix(float aspectRatio) {
    // Convert FOV from degrees to radians as cmath functions (like tan) expect radians
    float fovRadians = fov * (static_cast<float>(M_PI) / 180.0f);
    // Near and far clipping planes define the range of depths visible to the camera
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    return Mat4::perspective(fovRadians, aspectRatio, nearPlane, farPlane);
}

// Processes keyboard input to move the camera
void Camera::processKeyboard(const char* direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime; // Calculate movement distance based on speed and frame time

    // Update camera position based on the direction string
    if (strcmp(direction, "FORWARD") == 0)
        position = position + (front * velocity); // Move in the direction the camera is facing
    if (strcmp(direction, "BACKWARD") == 0)
        position = position - (front * velocity); // Move opposite to the direction the camera is facing
    if (strcmp(direction, "LEFT") == 0)
        position = position - (right * velocity); // Strafe left
    if (strcmp(direction, "RIGHT") == 0)
        position = position + (right * velocity); // Strafe right
    if (strcmp(direction, "UP") == 0)
        position = position + (worldUp * velocity); // Move vertically up (along world's Y-axis)
    if (strcmp(direction, "DOWN") == 0)
        position = position - (worldUp * velocity); // Move vertically down
}

// Processes mouse movement input to rotate the camera (adjust yaw and pitch)
void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    // Scale mouse movement by sensitivity
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;   // Adjust yaw (left/right rotation)
    pitch += yoffset; // Adjust pitch (up/down rotation)

    // Constrain pitch to prevent the camera from flipping upside down
    // A common range is -89 to +89 degrees.
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // Update the camera's front, right, and up vectors based on the new Euler angles
    updateCameraVectors();
}

// Processes mouse scroll wheel input, typically for zooming (adjusting FOV)
void Camera::processMouseScroll(float yoffset) {
    fov -= yoffset; // Adjust FOV; yoffset is usually 1 or -1 from scroll wheel

    // Constrain FOV to a reasonable range
    if (fov < 1.0f)
        fov = 1.0f;   // Minimum FOV
    if (fov > 75.0f) // Arbitrary maximum FOV, can be adjusted
        fov = 75.0f;
}

// Recalculates the camera's 'front', 'right', and 'up' vectors
// This is called after yaw or pitch changes.
void Camera::updateCameraVectors() {
    Vec3 newFront;
    // Convert yaw and pitch from degrees to radians for trigonometric functions
    float yawRad = yaw * (static_cast<float>(M_PI) / 180.0f);
    float pitchRad = pitch * (static_cast<float>(M_PI) / 180.0f);

    // Calculate the new front vector components using spherical coordinates
    newFront.x = std::cos(yawRad) * std::cos(pitchRad);
    newFront.y = std::sin(pitchRad);
    newFront.z = std::sin(yawRad) * std::cos(pitchRad);
    front = newFront.normalize(); // Ensure the front vector is a unit vector

    // Recalculate the right vector using cross product (front x worldUp)
    // This gives a vector perpendicular to both the front and world up vectors.
    right = Vec3::cross(front, worldUp).normalize();

    // Recalculate the camera's local up vector (right x front)
    // This ensures the up vector is always orthogonal to both right and front.
    up = Vec3::cross(right, front).normalize();
}
