#ifndef CAMERA_H
#define CAMERA_H

#include "../SimpleMath.h" // Assuming SimpleMath.h is in include/

struct GLFWwindow; // Forward declaration

class Camera {
public:
    // Camera Attributes
    Vec3 position;
    Vec3 front; // Direction camera is looking (normalized)
    Vec3 up;    // Camera's local up vector (orthogonal to front and right)
    Vec3 right; // Camera's local right vector (orthogonal to front and up)
    Vec3 worldUp; // Fixed world up direction (e.g., 0,1,0)

    // For Orbit Camera
    Vec3 focalPoint;
    float distanceToFocalPoint;

    // Euler Angles (representing camera's orientation around focal point for orbit)
    float yaw;   // Rotation around world Y-axis (degrees)
    float pitch; // Elevation from XZ plane of focal point (degrees)

    // Camera options
    float movementSpeed;    // For FPS-style movement
    float mouseSensitivity; // General sensitivity for FPS-like direct control
    float orbitSensitivity; // Sensitivity for orbiting with mouse
    float panSensitivity;   // Sensitivity for panning with mouse
    float zoomSensitivity;  // Sensitivity for zooming with scroll wheel
    float fov;              // Field of View in degrees

    enum class ProjectionMode {
        Perspective,
        Orthographic // Future consideration
    };
    ProjectionMode projectionMode;


    Camera(Vec3 initialPosition = Vec3(0.0f, 2.0f, 7.0f),
           Vec3 initialFocalPoint = Vec3(0.0f, 0.5f, 0.0f),
           Vec3 worldUpDirection = Vec3(0.0f, 1.0f, 0.0f));

    Mat4 getViewMatrix();
    Mat4 getProjectionMatrix(float aspectRatio);

    // FPS-style movement (for free-look when scene view is focused)
    void processKeyboardFPS(const char* direction, float deltaTime);

    // Editor camera controls (orbit, pan, zoom around/relative to focalPoint)
    void processMouseOrbit(float xoffset, float yoffset);
    void processMousePan(float xoffset, float yoffset);
    void processMouseZoom(float yoffsetScroll); // yoffsetScroll from mouse wheel

    // Updates camera vectors based on focalPoint, distanceToFocalPoint, yaw, and pitch
    void updateCameraVectors();

    // Call this if focal point is changed externally (e.g., selecting an object)
    void setFocalPoint(const Vec3& newFocalPoint);
};

#endif // CAMERA_H