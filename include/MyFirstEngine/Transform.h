#pragma once
// Transform.h
// A basic transform component holding position.
// In a full engine, this would also include rotation and scale.

#ifndef TRANSFORM_H
#define TRANSFORM_H

// We'll need a simple 3D vector. For now, let's define one.
// Later, you'd use a math library like GLM.
struct Vec3 {
    float x, y, z;

    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

struct Transform {
    Vec3 position;
    // Vec3 rotation; // Future addition
    // Vec3 scale;    // Future addition

    Transform() : position(0.0f, 0.0f, 0.0f) {}
};

#endif // TRANSFORM_H
