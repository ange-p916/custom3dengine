// Transform.h
// A basic transform component holding position, rotation, and scale.
// Uses Vec3 from SimpleMath.h.

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../SimpleMath.h" // Assuming Transform.h is in include/MyFirstEngine/
                           // and SimpleMath.h is in include/. This path means
                           // "go up one directory from MyFirstEngine to include/, then find SimpleMath.h"

struct Transform {
    Vec3 position;
    Vec3 rotation; // Euler angles (in radians or degrees, be consistent for your usage)
    Vec3 scale;

    // Constructor initializes to default values (e.g., no translation, no rotation, unit scale)
    Transform() : position(0.0f, 0.0f, 0.0f),
                  rotation(0.0f, 0.0f, 0.0f),
                  scale(1.0f, 1.0f, 1.0f) {}
};

#endif // TRANSFORM_H
