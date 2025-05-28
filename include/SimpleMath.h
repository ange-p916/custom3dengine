#ifndef SIMPLEMATH_H
#define SIMPLEMATH_H

#include <cmath>
#include <vector> 
#include <cstring> 
#include <algorithm> // For std::swap
#include <stdexcept> // For error handling if needed

// Define M_PI if not already defined
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Helper to convert degrees to radians
static float sm_toRadians(float degrees) {
    return degrees * (static_cast<float>(M_PI) / 180.0f);
}

// Forward declare GameObject for AABB constructor
struct GameObject; 

// --- Vec2 ---
struct Vec2 {
    float x, y;
    Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

// --- Vec3 ---
struct Vec3 {
    float x, y, z;
    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalize() const {
        float l = length();
        if (l > 1e-6f) { return Vec3(x / l, y / l, z / l); }
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    static float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
};

// --- Vec4 ---
struct Vec4 {
    float x, y, z, w;
    Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) : x(x), y(y), z(z), w(w) {}
    Vec4(const Vec3& v, float w_val) : x(v.x), y(v.y), z(v.z), w(w_val) {}
};

// --- Mat4 ---
struct Mat4 {
    float elements[16];

    Mat4(float diagonal = 1.0f) {
        for (int i = 0; i < 16; ++i) elements[i] = 0.0f;
        elements[0] = elements[5] = elements[10] = elements[15] = diagonal;
    }

    static Mat4 identity() { return Mat4(1.0f); }

    Mat4 operator*(const Mat4& other) const {
        Mat4 product(0.0f);
        for (int c = 0; c < 4; ++c) {
            for (int r = 0; r < 4; ++r) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += elements[k * 4 + r] * other.elements[c * 4 + k];
                }
                product.elements[c * 4 + r] = sum;
            }
        }
        return product;
    }
    
    Vec4 operator*(const Vec4& vec) const {
        Vec4 result;
        result.x = elements[0]*vec.x + elements[4]*vec.y + elements[8]*vec.z  + elements[12]*vec.w;
        result.y = elements[1]*vec.x + elements[5]*vec.y + elements[9]*vec.z  + elements[13]*vec.w;
        result.z = elements[2]*vec.x + elements[6]*vec.y + elements[10]*vec.z + elements[14]*vec.w;
        result.w = elements[3]*vec.x + elements[7]*vec.y + elements[11]*vec.z + elements[15]*vec.w;
        return result;
    }

    static Mat4 translate(const Vec3& translation) {
        Mat4 result = identity();
        result.elements[12] = translation.x;
        result.elements[13] = translation.y;
        result.elements[14] = translation.z;
        return result;
    }

    static Mat4 scale(const Vec3& scaleVec) {
        Mat4 result = identity();
        result.elements[0] = scaleVec.x;
        result.elements[5] = scaleVec.y;
        result.elements[10] = scaleVec.z;
        return result;
    }

    static Mat4 rotateX(float angleRadians) {
        Mat4 result = identity(); float c = std::cos(angleRadians); float s = std::sin(angleRadians);
        result.elements[5] = c;  result.elements[9] = -s;
        result.elements[6] = s;  result.elements[10] = c;
        return result;
    }

    static Mat4 rotateY(float angleRadians) {
        Mat4 result = identity(); float c = std::cos(angleRadians); float s = std::sin(angleRadians);
        result.elements[0] = c;   result.elements[8] = s;
        result.elements[2] = -s;  result.elements[10] = c;
        return result;
    }

    static Mat4 rotateZ(float angleRadians) {
        Mat4 result = identity(); float c = std::cos(angleRadians); float s = std::sin(angleRadians);
        result.elements[0] = c;   result.elements[4] = -s;
        result.elements[1] = s;   result.elements[5] = c;
        return result;
    }
    
    static Mat4 rotateEuler(const Vec3& eulerAnglesDegrees) {
        Mat4 rotZ = Mat4::rotateZ(sm_toRadians(eulerAnglesDegrees.z)); // Roll
        Mat4 rotX = Mat4::rotateX(sm_toRadians(eulerAnglesDegrees.x)); // Pitch
        Mat4 rotY = Mat4::rotateY(sm_toRadians(eulerAnglesDegrees.y)); // Yaw
        return rotY * rotX * rotZ; // Apply Yaw, then Pitch, then Roll
    }

    Mat4 inverse() const {
        Mat4 inv; float det;
        inv.elements[0] = elements[5]*elements[10]*elements[15] - elements[5]*elements[11]*elements[14] - elements[9]*elements[6]*elements[15] + elements[9]*elements[7]*elements[14] + elements[13]*elements[6]*elements[11] - elements[13]*elements[7]*elements[10];
        inv.elements[4] = -elements[4]*elements[10]*elements[15] + elements[4]*elements[11]*elements[14] + elements[8]*elements[6]*elements[15] - elements[8]*elements[7]*elements[14] - elements[12]*elements[6]*elements[11] + elements[12]*elements[7]*elements[10];
        inv.elements[8] = elements[4]*elements[9]*elements[15] - elements[4]*elements[11]*elements[13] - elements[8]*elements[5]*elements[15] + elements[8]*elements[7]*elements[13] + elements[12]*elements[5]*elements[11] - elements[12]*elements[7]*elements[9];
        inv.elements[12] = -elements[4]*elements[9]*elements[14] + elements[4]*elements[10]*elements[13] + elements[8]*elements[5]*elements[14] - elements[8]*elements[6]*elements[13] - elements[12]*elements[5]*elements[10] + elements[12]*elements[6]*elements[9];
        inv.elements[1] = -elements[1]*elements[10]*elements[15] + elements[1]*elements[11]*elements[14] + elements[9]*elements[2]*elements[15] - elements[9]*elements[3]*elements[14] - elements[13]*elements[2]*elements[11] + elements[13]*elements[3]*elements[10];
        inv.elements[5] = elements[0]*elements[10]*elements[15] - elements[0]*elements[11]*elements[14] - elements[8]*elements[2]*elements[15] + elements[8]*elements[3]*elements[14] + elements[12]*elements[2]*elements[11] - elements[12]*elements[3]*elements[10];
        inv.elements[9] = -elements[0]*elements[9]*elements[15] + elements[0]*elements[11]*elements[13] + elements[8]*elements[1]*elements[15] - elements[8]*elements[3]*elements[13] - elements[12]*elements[1]*elements[11] + elements[12]*elements[3]*elements[9];
        inv.elements[13] = elements[0]*elements[9]*elements[14] - elements[0]*elements[10]*elements[13] - elements[8]*elements[1]*elements[14] + elements[8]*elements[2]*elements[13] + elements[12]*elements[1]*elements[10] - elements[12]*elements[2]*elements[9];
        inv.elements[2] = elements[1]*elements[6]*elements[15] - elements[1]*elements[7]*elements[14] - elements[5]*elements[2]*elements[15] + elements[5]*elements[3]*elements[14] + elements[13]*elements[2]*elements[7] - elements[13]*elements[3]*elements[6];
        inv.elements[6] = -elements[0]*elements[6]*elements[15] + elements[0]*elements[7]*elements[14] + elements[4]*elements[2]*elements[15] - elements[4]*elements[3]*elements[14] - elements[12]*elements[2]*elements[7] + elements[12]*elements[3]*elements[6];
        inv.elements[10] = elements[0]*elements[5]*elements[15] - elements[0]*elements[7]*elements[13] - elements[4]*elements[1]*elements[15] + elements[4]*elements[3]*elements[13] + elements[12]*elements[1]*elements[7] - elements[12]*elements[3]*elements[5];
        inv.elements[14] = -elements[0]*elements[5]*elements[14] + elements[0]*elements[6]*elements[13] + elements[4]*elements[1]*elements[14] - elements[4]*elements[2]*elements[13] - elements[12]*elements[1]*elements[6] + elements[12]*elements[2]*elements[5];
        inv.elements[3] = -elements[1]*elements[6]*elements[11] + elements[1]*elements[7]*elements[10] + elements[5]*elements[2]*elements[11] - elements[5]*elements[3]*elements[10] - elements[9]*elements[2]*elements[7] + elements[9]*elements[3]*elements[6];
        inv.elements[7] = elements[0]*elements[6]*elements[11] - elements[0]*elements[7]*elements[10] - elements[4]*elements[2]*elements[11] + elements[4]*elements[3]*elements[10] + elements[8]*elements[2]*elements[7] - elements[8]*elements[3]*elements[6];
        inv.elements[11] = -elements[0]*elements[5]*elements[11] + elements[0]*elements[7]*elements[9] + elements[4]*elements[1]*elements[11] - elements[4]*elements[3]*elements[9] - elements[8]*elements[1]*elements[7] + elements[8]*elements[3]*elements[5];
        inv.elements[15] = elements[0]*elements[5]*elements[10] - elements[0]*elements[6]*elements[9] - elements[4]*elements[1]*elements[10] + elements[4]*elements[2]*elements[9] + elements[8]*elements[1] *elements[6] - elements[8]*elements[2]*elements[5];
        det = elements[0]*inv.elements[0] + elements[1]*inv.elements[4] + elements[2]*inv.elements[8] + elements[3]*inv.elements[12];
        if (std::abs(det) < 1e-6f) return identity(); // Matrix is singular or near-singular
        det = 1.0f / det;
        for (int i = 0; i < 16; i++) inv.elements[i] = inv.elements[i] * det;
        return inv;
    }
    
    static Vec3 transformPoint(const Mat4& mat, const Vec3& point) {
        Vec4 result_v4 = mat * Vec4(point, 1.0f);
        if (std::abs(result_v4.w) > 1e-6f && result_v4.w != 1.0f) { // Check for non-zero w before division
            return Vec3(result_v4.x / result_v4.w, result_v4.y / result_v4.w, result_v4.z / result_v4.w);
        }
        return Vec3(result_v4.x, result_v4.y, result_v4.z);
    }

    static Vec3 transformDirection(const Mat4& mat, const Vec3& dir) {
        Vec4 result_v4 = mat * Vec4(dir, 0.0f);
        return Vec3(result_v4.x, result_v4.y, result_v4.z);
    }

    static Mat4 perspective(float fovYRadians, float aspect, float nearZ, float farZ) {
        Mat4 result(0.0f);
        const float tanHalfFovY = std::tan(fovYRadians / 2.0f);
        result.elements[0] = 1.0f / (aspect * tanHalfFovY);
        result.elements[5] = 1.0f / (tanHalfFovY);
        result.elements[10] = -(farZ + nearZ) / (farZ - nearZ);
        result.elements[11] = -1.0f;
        result.elements[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
        return result;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& worldUp) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = Vec3::cross(f, worldUp).normalize();
        // Recompute actual up vector to ensure orthogonality
        Vec3 u = Vec3::cross(s, f).normalize(); 
        Mat4 result = identity();
        result.elements[0] = s.x;  result.elements[4] = s.y;  result.elements[8] = s.z;
        result.elements[1] = u.x;  result.elements[5] = u.y;  result.elements[9] = u.z;
        result.elements[2] = -f.x; result.elements[6] = -f.y; result.elements[10] = -f.z;
        result.elements[12] = -Vec3::dot(s, eye);
        result.elements[13] = -Vec3::dot(u, eye);
        result.elements[14] = Vec3::dot(f, eye); // Original was -Vec3::dot(-f, eye) which is Vec3::dot(f, eye)
        return result;
    }

    const float* getElementsPtr() const {
        return elements;
    }
};


// Ray Structure
struct Ray {
    Vec3 origin;
    Vec3 direction; // Should be normalized

    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d.normalize()) {}
};

// AABB defined by min and max points
struct AABB {
    Vec3 min;
    Vec3 max;

    AABB(const Vec3& cen = Vec3(0,0,0), const Vec3& halfExtents = Vec3(0.5f,0.5f,0.5f)) {
        min = cen - halfExtents;
        max = cen + halfExtents;
    }
    // Constructor from GameObject (simplified - assumes scale is full extents and object is axis aligned in world AFTER translation)
    // For more accuracy with rotated objects, this needs to transform local AABB by model matrix.
    AABB(const GameObject& go); // Declaration only, definition needs GameObject.h
};

// Ray-AABB intersection
// Returns true if intersection occurs, t is the distance along the ray to the FIRST intersection point
inline bool intersectRayAABB(const Ray& ray, const AABB& box, float& t) {
    Vec3 invDir(1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z);

    Vec3 t0s = (box.min - ray.origin) * invDir.x; // Not quite right, needs component-wise
    Vec3 t1s = (box.max - ray.origin) * invDir.x; // Not quite right

    // Correct component-wise multiplication for t values
    float tx1 = (box.min.x - ray.origin.x) * invDir.x;
    float tx2 = (box.max.x - ray.origin.x) * invDir.x;
    float ty1 = (box.min.y - ray.origin.y) * invDir.y;
    float ty2 = (box.max.y - ray.origin.y) * invDir.y;
    float tz1 = (box.min.z - ray.origin.z) * invDir.z;
    float tz2 = (box.max.z - ray.origin.z) * invDir.z;

    float tmin = std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)), std::min(tz1, tz2));
    float tmax = std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)), std::max(tz1, tz2));

    if (tmax < 0 || tmin > tmax) { // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
        t = tmax;
        return false;
    }

    t = tmin; //
    if (tmin < 0) { // if tmin < 0, ray origin is inside AABB
      t = tmax; // consider hit at exit point if needed, or t = 0 for "inside"
      if (tmax < 0) return false; // both entry and exit are behind
      // for picking, if origin is inside, we usually want t=0 or the exit point.
      // For now, let's stick to the first positive intersection.
      // If tmin < 0, it means the ray starts inside the box or the box is behind the ray.
      // We are interested in the first intersection point in front of the ray origin.
      // t = tmin; // This will be negative if origin is outside and box is behind.
      // return true; // A hit occurs
      // A common way:
      // t_enter = tmin, t_exit = tmax
      // if t_enter > t_exit or t_exit < 0 then no collision
      // if t_enter < 0 then ray origin inside box, collision at t = 0 (or t_enter if you want distance)
      // else collision at t_enter
      t = tmin;
      return true;
    }
    
    return true;
}


#endif // SIMPLEMATH_H