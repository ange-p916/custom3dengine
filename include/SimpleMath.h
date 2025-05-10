// SimpleMath.h
// Very basic math structures (Vec3, Mat4) and functions.
// For a real engine, using a dedicated library like GLM is highly recommended.

#ifndef SIMPLEMATH_H
#define SIMPLEMATH_H

#include <cmath> // For sin, cos, tan, sqrt
#include <vector> // Not strictly needed for Mat4 as a flat array, but good for general C++
#include <cstring> // For memcpy if used (not used in current Mat4)
#include <stdexcept> // For potential error handling (not used yet)

// --- Vec3 ---
struct Vec3 {
    float x, y, z;

    // Default constructor
    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    // Vector addition
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    // Vector subtraction
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Scalar multiplication
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Dot product
    static float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Cross product
    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    // Magnitude (length) of the vector
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Returns a normalized version of the vector (unit vector)
    Vec3 normalize() const {
        float l = length();
        // Check for zero length to avoid division by zero
        if (l > 1e-6f) { // Use a small epsilon for floating point comparison
            return Vec3(x / l, y / l, z / l);
        }
        return Vec3(0.0f, 0.0f, 0.0f); // Return zero vector if length is too small
    }
};


// --- Mat4 ---
// Represents a 4x4 Matrix, typically used for transformations in 3D graphics.
// Stored in column-major order to be directly compatible with OpenGL.
struct Mat4 {
    // Elements are stored as:
    // col0x col1x col2x col3x (elements[0] elements[4] elements[8]  elements[12])
    // col0y col1y col2y col3y (elements[1] elements[5] elements[9]  elements[13])
    // col0z col1z col2z col3z (elements[2] elements[6] elements[10] elements[14])
    // col0w col1w col2w col3w (elements[3] elements[7] elements[11] elements[15])
    float elements[16];

    // Constructor: initializes to identity matrix by default, or with a diagonal value.
    Mat4(float diagonal = 1.0f) {
        for (int i = 0; i < 16; ++i) elements[i] = 0.0f;
        elements[0]  = diagonal; // Top-left
        elements[5]  = diagonal; // Middle
        elements[10] = diagonal; // Middle
        elements[15] = diagonal; // Bottom-right
    }

    // Returns an identity matrix.
    static Mat4 identity() {
        return Mat4(1.0f);
    }

    // Matrix multiplication (this * other)
    Mat4 operator*(const Mat4& other) const {
        Mat4 product(0.0f); // Initialize result matrix with zeros
        for (int c = 0; c < 4; ++c) { // Iterate over columns of the result matrix (and 'other' matrix)
            for (int r = 0; r < 4; ++r) { // Iterate over rows of the result matrix (and 'this' matrix)
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    // Result[col][row] = sum(this[k][row] * other[col][k])
                    // Accessing column-major: elements[column_index * 4 + row_index]
                    sum += elements[k * 4 + r] * other.elements[c * 4 + k];
                }
                product.elements[c * 4 + r] = sum;
            }
        }
        return product;
    }

    // Creates a translation matrix.
    static Mat4 translate(const Vec3& translation) {
        Mat4 result = Mat4::identity();
        // Translation components go into the last column (elements 12, 13, 14)
        result.elements[12] = translation.x; // Column 3, Row 0
        result.elements[13] = translation.y; // Column 3, Row 1
        result.elements[14] = translation.z; // Column 3, Row 2
        return result;
    }

    // Creates a perspective projection matrix.
    // fovYRadians: field of view in the Y direction, in radians.
    // aspect: aspect ratio (width / height).
    // nearZ, farZ: distances to the near and far clipping planes.
    static Mat4 perspective(float fovYRadians, float aspect, float nearZ, float farZ) {
        Mat4 result(0.0f); // Initialize with all zeros
        const float tanHalfFovY = std::tan(fovYRadians / 2.0f);

        result.elements[0] = 1.0f / (aspect * tanHalfFovY);       // Scale X
        result.elements[5] = 1.0f / (tanHalfFovY);                // Scale Y

        result.elements[10] = -(farZ + nearZ) / (farZ - nearZ);   // Used to map Z to [-1, 1]
        result.elements[11] = -1.0f;                              // Puts Z into W for perspective division

        result.elements[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ); // Translation for Z mapping
        // result.elements[15] = 0.0f; // W component of output is 0 (already set by constructor)

        return result;
    }

    // Creates a lookAt view matrix.
    // eye: camera position.
    // center: point the camera is looking at.
    // worldUp: world's up direction (e.g., (0,1,0)).
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& worldUp) {
        Vec3 f = (center - eye).normalize();      // Forward vector (direction camera is looking)
        Vec3 s = Vec3::cross(f, worldUp).normalize(); // Right vector
        Vec3 u = Vec3::cross(s, f);               // Up vector (camera's local up)

        Mat4 result = Mat4::identity();

        // Set the rotation part of the matrix
        result.elements[0] = s.x;   result.elements[4] = s.y;   result.elements[8] = s.z;
        result.elements[1] = u.x;   result.elements[5] = u.y;   result.elements[9] = u.z;
        result.elements[2] = -f.x;  result.elements[6] = -f.y;  result.elements[10] = -f.z; // Negate f because OpenGL looks down -Z

        // Set the translation part of the matrix
        result.elements[12] = -Vec3::dot(s, eye);
        result.elements[13] = -Vec3::dot(u, eye);
        result.elements[14] = Vec3::dot(f, eye); // Dot product with -(-f)

        return result;
    }

    // Returns a const pointer to the matrix's elements, for OpenGL functions.
    const float* getElementsPtr() const {
        return elements;
    }
};

#endif // SIMPLEMATH_H
