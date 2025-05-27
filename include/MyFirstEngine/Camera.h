// Camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "../SimpleMath.h"

struct GLFWwindow; // Forward declaration

class Camera {
public:
    // Camera Attributes
    Vec3 position;
    Vec3 front;
    Vec3 up;
    Vec3 right;
    Vec3 worldUp;

    // For Orbit Camera
    Vec3 focalPoint;
    float distanceToFocalPoint;

    // Euler Angles (still useful for FPS or if you want to combine)
    float yaw;
    float pitch;

    // Camera options
    float movementSpeed;    // For FPS-style movement (can be kept for free-look)
    float mouseSensitivity;
    float orbitSensitivity; // New: for orbit speed
    float panSensitivity;   // New: for pan speed
    float zoomSensitivity;  // New: for zoom speed
    float fov;

    enum class ProjectionMode {
        Perspective,
        Orthographic // Future consideration
    };
    ProjectionMode projectionMode;


    Camera(Vec3 position = Vec3(0.0f, 1.0f, 5.0f), // Adjusted default start
           Vec3 focus = Vec3(0.0f, 0.0f, 0.0f),  // New: focal point
           Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
           float initialYaw = -90.0f, float initialPitch = 0.0f);

    Mat4 getViewMatrix();
    Mat4 getProjectionMatrix(float aspectRatio);

    // FPS-style movement (keep if you want a free-look mode)
    void processKeyboardFPS(const char* direction, float deltaTime);

    // Editor camera controls
    void processMouseOrbit(float xoffset, float yoffset);
    void processMousePan(float xoffset, float yoffset);
    void processMouseZoom(float yoffset); // Renamed from processMouseScroll for clarity

    // Updates camera vectors based on current mode (FPS or Orbit)
    void updateCameraVectors(); // This will need to be smarter or have different versions

private:
    void updateOrbitingCameraVectors(); // Helper for orbit mode
    void updateFPSCameraVectors();      // Helper for FPS mode (your old updateCameraVectors)
};

#endif // CAMERA_H