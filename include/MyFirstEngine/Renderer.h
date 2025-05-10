// Renderer.h
// Declaration of the Renderer class.
// Now handles vertices with position and color attributes.

#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h" // Needs Shader class

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initializes shaders, buffers, and vertex array objects
    // Returns true on success, false on failure.
    bool init();

    // Renders the scene (currently a colored triangle)
    void draw();

private:
    unsigned int VAO;         // Vertex Array Object ID
    unsigned int VBO;         // Vertex Buffer Object ID
    Shader* shaderProgram;    // Pointer to our shader program object

    // Triangle vertices:
    // Each vertex now has 6 components:
    // X,  Y,  Z,    R,  G,  B
    // (Position)   (Color)
    float vertices[18] = {
        // Positions         // Colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // Bottom-left (Red)
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // Bottom-right (Green)
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // Top-center (Blue)
    };
};

#endif // RENDERER_H
