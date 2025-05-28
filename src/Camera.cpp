#include "MyFirstEngine/Camera.h" // Adjust path if necessary
#include <cmath>
#include <algorithm> // For std::max, std::min
#include <cstring>   // For strcmp
#include <iostream>  // For debugging (optional)

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Helper to convert degrees to radians (already in SimpleMath.h but can be local too)
static float toRadians(float degrees) {
    return degrees * (static_cast<float>(M_PI) / 180.0f);
}

Camera::Camera(Vec3 initialPosition, Vec3 initialFocalPoint, Vec3 worldUpDirection)
    : position(initialPosition),
      worldUp(worldUpDirection.normalize()),
      focalPoint(initialFocalPoint),
      movementSpeed(3.5f),      // Slightly increased default speed
      mouseSensitivity(0.1f),
      orbitSensitivity(0.25f),  // Degrees per pixel delta
      panSensitivity(0.0025f),  // World units per pixel delta (scaled by distance)
      zoomSensitivity(0.8f),    // Change in distance per scroll unit
      fov(45.0f),
      projectionMode(ProjectionMode::Perspective) {

    // Calculate initial distance, yaw, and pitch from the initial position and focal point
    Vec3 dirToCamera = position - focalPoint;
    distanceToFocalPoint = dirToCamera.length();

    if (distanceToFocalPoint < 0.001f) { // Avoid camera being exactly at the focal point
        // Move camera slightly along an arbitrary axis if it's at the focal point
        position = focalPoint + Vec3(0.0f, 0.0f, 1.0f); 
        dirToCamera = position - focalPoint;
        distanceToFocalPoint = dirToCamera.length();
    }

    Vec3 dirToCameraNorm = dirToCamera.normalize();

    // Pitch: angle with the XZ plane (focal point's horizontal plane)
    // asin's range is [-PI/2, PI/2], so pitch in degrees will be [-90, 90]
    pitch = std::asin(dirToCameraNorm.y) * (180.0f / static_cast<float>(M_PI));

    // Yaw: angle in the XZ plane relative to some axis (e.g., positive Z)
    // atan2(x, z) gives angle from +Z axis. Positive yaw = rotate right (clockwise looking down Y).
    // Or atan2(z, x) for angle from +X axis.
    // Let's use atan2(x, z) to be somewhat consistent with common yaw definitions (0 along +Z)
    float xz_len = std::sqrt(dirToCameraNorm.x * dirToCameraNorm.x + dirToCameraNorm.z * dirToCameraNorm.z);
    if (xz_len > 0.001f) {
        // Yaw: Angle of projection on XZ plane from the Z-axis towards X-axis.
        // If camera is at (0,y,dist) from focal (0,y,0), dirToCameraNorm = (0,0,1). atan2(0,1) = 0.
        // If camera is at (dist,y,0) from focal (0,y,0), dirToCameraNorm = (1,0,0). atan2(1,0) = 90.
        yaw = std::atan2(dirToCameraNorm.x, dirToCameraNorm.z) * (180.0f / static_cast<float>(M_PI));
    } else {
        yaw = 0.0f; // Looking straight up or down, yaw is undefined/arbitrary
    }

    updateCameraVectors(); // Initial calculation of front, right, up based on these angles
}

void Camera::updateCameraVectors() {
    // Calculate camera's new position based on focal point, distance, yaw, and pitch
    float radPitch = toRadians(pitch);
    float radYaw = toRadians(yaw);

    // Spherical to Cartesian conversion for the offset from the focal point
    // x = r * cos(pitch) * sin(yaw)
    // y = r * sin(pitch)
    // z = r * cos(pitch) * cos(yaw)
    // (This assumes yaw=0 means camera is along +Z from focal, pitch=0 is level)
    position.x = focalPoint.x + distanceToFocalPoint * std::cos(radPitch) * std::sin(radYaw);
    position.y = focalPoint.y + distanceToFocalPoint * std::sin(radPitch);
    position.z = focalPoint.z + distanceToFocalPoint * std::cos(radPitch) * std::cos(radYaw);

    // Recalculate front, right, and up vectors
    front = (focalPoint - position).normalize(); // Camera always looks towards the focal point
    right = Vec3::cross(front, worldUp).normalize();
    up = Vec3::cross(right, front).normalize(); // Camera's local up vector
}

void Camera::setFocalPoint(const Vec3& newFocalPoint) {
    Vec3 oldPosition = position;
    focalPoint = newFocalPoint;
    
    // Option 1: Keep the same distance and orientation relative to the new focal point
    // This means the camera "jumps" to maintain the same viewing angle and distance
    // from the new target.
    // updateCameraVectors(); 

    // Option 2: Keep current camera position, just update yaw/pitch/distance to look at new focal point
    // This feels more natural if an object is selected and camera just re-orients.
    Vec3 dirToCamera = oldPosition - focalPoint;
    distanceToFocalPoint = dirToCamera.length();

    if (distanceToFocalPoint < 0.01f) { // If camera is too close to new focal point
        // Move camera slightly back along its current front vector to maintain a minimum distance
        // This prevents issues if the new focal point is where the camera currently is.
        // We need a 'front' vector before this logic.
        // Let's recalculate initial front based on old position and new focal point
        Vec3 tempFront = (focalPoint - oldPosition).normalize();
        if (tempFront.length() < 0.001f) tempFront = Vec3(0,0,-1); // Default if still coincident

        position = focalPoint - (tempFront * 1.0f); // Default distance of 1
        dirToCamera = position - focalPoint;
        distanceToFocalPoint = dirToCamera.length();
    }
    
    Vec3 dirToCameraNorm = dirToCamera.normalize();
    pitch = std::asin(dirToCameraNorm.y) * (180.0f / static_cast<float>(M_PI));
    float xz_len = std::sqrt(dirToCameraNorm.x * dirToCameraNorm.x + dirToCameraNorm.z * dirToCameraNorm.z);
    if (xz_len > 0.001f) {
        yaw = std::atan2(dirToCameraNorm.x, dirToCameraNorm.z) * (180.0f / static_cast<float>(M_PI));
    } // else keep old yaw if looking straight up/down

    updateCameraVectors(); // This updates position based on new yaw/pitch/dist to focal, AND front/right/up
}


Mat4 Camera::getViewMatrix() {
    // For an orbit camera, view matrix is always looking from 'position' to 'focalPoint'
    return Mat4::lookAt(position, focalPoint, worldUp);
}

Mat4 Camera::getProjectionMatrix(float aspectRatio) {
    if (aspectRatio <= 0.0f) aspectRatio = 1.0f; // Prevent division by zero or negative aspect
    float fovRad = toRadians(fov);
    float nearPlane = 0.1f;
    float farPlane = 1000.0f; // Increased far plane for larger scenes
    return Mat4::perspective(fovRad, aspectRatio, nearPlane, farPlane);
}

void Camera::processKeyboardFPS(const char* direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    Vec3 moveDirection(0.0f, 0.0f, 0.0f);

    // For FPS movement, we use the camera's current orientation (front, right, worldUp)
    // Note: 'front' for an orbit camera points towards the focal point.
    // If you want classic FPS where 'front' is where you're looking irrespective of a focal point,
    // this FPS movement will move the camera AND its focal point together.
    if (strcmp(direction, "FORWARD") == 0) moveDirection = moveDirection + front;
    if (strcmp(direction, "BACKWARD") == 0) moveDirection = moveDirection - front;
    if (strcmp(direction, "LEFT") == 0) moveDirection = moveDirection - right;
    if (strcmp(direction, "RIGHT") == 0) moveDirection = moveDirection + right;
    if (strcmp(direction, "UP") == 0) moveDirection = moveDirection + worldUp; // Move along world Y
    if (strcmp(direction, "DOWN") == 0) moveDirection = moveDirection - worldUp; // Move along world Y

    if (moveDirection.length() > 0.001f) {
        Vec3 displacement = moveDirection.normalize() * velocity;
        position = position + displacement;
        focalPoint = focalPoint + displacement; // Move focal point along with camera for FPS-like free look
        // No need to update yaw/pitch/distance, as they are relative to focal point.
        // updateCameraVectors() isn't strictly needed here if focal point moves with position,
        // but front/right/up might need re-orthogonalization if not perfect.
        // However, our getViewMatrix uses position and focalPoint, so it remains correct.
        // Let's ensure front/right/up are updated for consistency if other systems use them.
        this->front = (this->focalPoint - this->position).normalize(); // Keep front pointing to focal
        this->right = Vec3::cross(this->front, this->worldUp).normalize();
        this->up    = Vec3::cross(this->right, this->front).normalize();
    }
}

void Camera::processMouseOrbit(float xoffset, float yoffset) {
    yaw -= xoffset * orbitSensitivity;   // Adjust yaw (horizontal orbit)
    pitch += yoffset * orbitSensitivity; // Adjust pitch (vertical orbit) - sign might depend on preference

    // Clamp pitch to avoid flipping
    pitch = std::max(-89.9f, std::min(89.9f, pitch)); // Avoid gimbal lock at exactly +/-90

    updateCameraVectors(); // Recalculate position and orientation vectors
}

void Camera::processMousePan(float xoffset, float yoffset) {
    // Pan speed can be scaled by distance for more intuitive control
    float effectivePanSpeed = panSensitivity * (distanceToFocalPoint * 0.2f); // Adjust 0.2f as needed
    effectivePanSpeed = std::max(0.001f, effectivePanSpeed); // Minimum pan speed

    // Move camera and focal point by the same amount along camera's right and up axes
    Vec3 panRight = right * (-xoffset * effectivePanSpeed); // Invert xoffset for natural pan
    Vec3 panUp    = up    * ( yoffset * effectivePanSpeed); // yoffset might need inversion depending on mouse coords

    position   = position + panRight + panUp;
    focalPoint = focalPoint + panRight + panUp;

    // After panning, the relative orientation (yaw/pitch/distance) to the focal point has not changed,
    // so updateCameraVectors() is not strictly needed to recalculate position from those angles.
    // However, we need to ensure `front`, `right`, and `up` are correct for subsequent operations.
    // Since `position` and `focalPoint` both moved, the `front` vector is still valid.
    // `right` and `up` depend on `front` and `worldUp`, so they should also be fine.
    // For safety, or if drift occurs:
    // updateCameraVectors(); // This would re-derive yaw/pitch from new pos/focal and then update pos again.
    // A simpler update might be better if only panning:
    this->front = (this->focalPoint - this->position).normalize(); // Keep front pointing to focal
    this->right = Vec3::cross(this->front, this->worldUp).normalize();
    this->up    = Vec3::cross(this->right, this->front).normalize();
}

void Camera::processMouseZoom(float yoffsetScroll) {
    // yoffsetScroll is typically +1 or -1 from mouse wheel
    distanceToFocalPoint -= yoffsetScroll * zoomSensitivity;
    distanceToFocalPoint = std::max(0.1f, distanceToFocalPoint); // Clamp minimum distance

    updateCameraVectors(); // Recalculate position based on new distance, and update vectors
}