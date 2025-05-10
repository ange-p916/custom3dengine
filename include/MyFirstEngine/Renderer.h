// Renderer.h
// Declaration of the Renderer class.
// Now handles vertices with position and color attributes,
// and can accept a transform for the object being drawn.

#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"     // Needs Shader class
#include "Transform.h"  // For Vec3 (used as parameter in draw and for uniform in Shader)
                        // Make sure Transform.h is in your include path
                        // (e.g., in include/MyFirstEngine/ or directly in include/)

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initializes shaders, buffers, and vertex array objects
    // Returns true on success, false on failure.
    bool init();

    // Renders the scene (currently a colored triangle)
    // Accepts a position to offset the drawing.
    void draw(const Vec3& position); // <-- Ensure this parameter is present

private:
    unsigned int VAO;         // Vertex Array Object ID
    unsigned int VBO;         // Vertex Buffer Object ID
    Shader* shaderProgram;    // Pointer to our shader program object

    // Triangle vertices: (Position X,Y,Z + Color R,G,B)
    float vertices[18] = {
        // Positions         // Colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // Bottom-left (Red)
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // Bottom-right (Green)
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // Top-center (Blue)
    };
};

#endif // RENDERER_H
