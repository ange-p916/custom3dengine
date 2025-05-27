// Camera.cpp
#include "MyFirstEngine/Camera.h"
#include <cmath>
#include <algorithm>
#include <cstring> // For strcmp in processKeyboardFPS
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Convert degrees to radians
float toRadians(float degrees) {
    return degrees * (static_cast<float>(M_PI) / 180.0f);
}

Camera::Camera(Vec3 position, Vec3 focus, Vec3 up, float initialYaw, float initialPitch)
    : front(Vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(2.5f),
      mouseSensitivity(0.1f),
      orbitSensitivity(0.3f), // Adjust these as needed
      panSensitivity(0.005f), // Adjust these as needed
      zoomSensitivity(1.0f),  // Adjust these as needed
      fov(45.0f),
      projectionMode(ProjectionMode::Perspective) { // Default to perspective

    this->position = position;
    this->worldUp = up;
    this->focalPoint = focus;

    // For FPS mode initial orientation (if used)
    this->yaw = initialYaw;
    this->pitch = initialPitch;

    // For Orbit mode, calculate initial distance and orientation
    this->front = (this->focalPoint - this->position).normalize();
    this->distanceToFocalPoint = (this->focalPoint - this->position).length();
    
    // Calculate initial yaw and pitch for orbit based on position and focalPoint
    // This makes the initial yaw/pitch consistent with the starting orientation
    Vec3 direction = (this->focalPoint - this->position).normalize();
    this->yaw = std::atan2(direction.z, direction.x) * (180.0f / static_cast<float>(M_PI));
    this->pitch = std::asin(direction.y) * (180.0f / static_cast<float>(M_PI));


    updateCameraVectors(); // Initial calculation of right/up
}

Mat4 Camera::getViewMatrix() {
    // For an orbiting camera, position is determined by focalPoint, distance, yaw, and pitch
    // For FPS, it's just eye, eye + front, worldUp
    // We'll use a unified approach for now: position and front are always maintained
    return Mat4::lookAt(position, position + front, worldUp);
}

Mat4 Camera::getProjectionMatrix(float aspectRatio) {
    if (aspectRatio <= 0) aspectRatio = 1.0f; // Prevent division by zero or negative
    float fovRadians = toRadians(fov);
    float nearPlane = 0.1f;
    float farPlane = 1000.0f; // Increased far plane
    return Mat4::perspective(fovRadians, aspectRatio, nearPlane, farPlane);
}

void Camera::processKeyboardFPS(const char* direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (strcmp(direction, "FORWARD") == 0) position = position + (front * velocity);
    if (strcmp(direction, "BACKWARD") == 0) position = position - (front * velocity);
    if (strcmp(direction, "LEFT") == 0) position = position - (right * velocity);
    if (strcmp(direction, "RIGHT") == 0) position = position + (right * velocity);
    if (strcmp(direction, "UP") == 0) position = position + (worldUp * velocity); // Using worldUp for simple vertical
    if (strcmp(direction, "DOWN") == 0) position = position - (worldUp * velocity);
    // After FPS movement, if we want to keep orbiting around the same focal point,
    // we might need to update focalPoint or re-evaluate distance.
    // For a true free-look FPS mode, focalPoint isn't relevant during movement.
}

void Camera::processMouseOrbit(float xoffset, float yoffset) {
    xoffset *= orbitSensitivity;
    yoffset *= orbitSensitivity;

    yaw += xoffset;
    pitch -= yoffset; // Reversed for typical orbit controls (mouse up = camera goes up)

    // Constrain pitch
    pitch = std::max(-89.0f, std::min(89.0f, pitch));

    updateOrbitingCameraVectors();
}

void Camera::processMousePan(float xoffset, float yoffset) {
    // Pan moves the camera and the focal point together
    Vec3 rightDisplacement = right * (-xoffset * panSensitivity * (distanceToFocalPoint / 5.0f)); // Scale pan with distance
    Vec3 upDisplacement = up * (yoffset * panSensitivity * (distanceToFocalPoint / 5.0f));       // Scale pan with distance

    position = position + rightDisplacement + upDisplacement;
    focalPoint = focalPoint + rightDisplacement + upDisplacement;

    updateCameraVectors(); // Recalculate front based on new position relative to focal point
}

void Camera::processMouseZoom(float yoffset) {
    distanceToFocalPoint -= yoffset * zoomSensitivity;
    distanceToFocalPoint = std::max(0.1f, distanceToFocalPoint); // Prevent going through or behind focal point

    // Recalculate position based on new distance, keeping current orientation around focal point
    position = focalPoint - (front * distanceToFocalPoint);
    // No need to call updateCameraVectors if only distance changes and front vector direction is maintained
    // However, if zoom changes FOV:
    // fov -= yoffset * zoomSensitivity;
    // fov = std::max(1.0f, std::min(90.0f, fov));
}


void Camera::updateCameraVectors() {
    // This function will now primarily update for an orbit-style camera,
    // or you can have a mode switch. For simplicity, let's make it orbit-centric.
    updateOrbitingCameraVectors();
}


void Camera::updateOrbitingCameraVectors() {
    // Calculate the new 'front' vector from Euler angles (yaw, pitch)
    // This defines the direction from the focal point to the camera
    Vec3 direction;
    direction.x = std::cos(toRadians(yaw)) * std::cos(toRadians(pitch));
    direction.y = std::sin(toRadians(pitch));
    direction.z = std::sin(toRadians(yaw)) * std::cos(toRadians(pitch));
    
    // The camera's 'front' vector should point from the camera towards the focal point.
    // So, if 'direction' points from focal point to camera, then 'front' is -direction.
    // However, it's often more intuitive to define yaw/pitch as the camera's orientation.
    // Let's assume yaw/pitch orient the vector FROM focal point TO camera.
    // Then, position is focalPoint + direction_normalized * distance.
    // And 'front' vector (what camera looks at) is (focalPoint - position).normalize()

    position = focalPoint - (direction.normalize() * distanceToFocalPoint);

    front = (focalPoint - position).normalize();
    right = Vec3::cross(front, worldUp).normalize();
    up = Vec3::cross(right, front).normalize(); // Camera's local up
}


// Keep your old FPS update logic if you want a toggleable FPS mode
void Camera::updateFPSCameraVectors() {
    Vec3 newFront;
    newFront.x = std::cos(toRadians(yaw)) * std::cos(toRadians(pitch));
    newFront.y = std::sin(toRadians(pitch));
    newFront.z = std::sin(toRadians(yaw)) * std::cos(toRadians(pitch));
    front = newFront.normalize();
    right = Vec3::cross(front, worldUp).normalize();
    up = Vec3::cross(right, front).normalize();
}