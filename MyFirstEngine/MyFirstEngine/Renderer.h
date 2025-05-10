// Renderer.h
// Declaration of the Renderer class.
// Encapsulates basic OpenGL rendering setup and draw calls for a triangle.

#ifndef RENDERER_H
#define RENDERER_H

// Forward declaration of Shader class to avoid circular dependency if Shader.h included Renderer.h
// However, in this simple case, Renderer uses Shader, so including Shader.h is fine.
#include "Shader.h" // Needs Shader class

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initializes shaders, buffers, and vertex array objects
    // Returns true on success, false on failure.
    bool init();

    // Renders the scene (currently just a triangle)
    void draw();

private:
    unsigned int VAO;         // Vertex Array Object ID
    unsigned int VBO;         // Vertex Buffer Object ID
    Shader* shaderProgram;    // Pointer to our shader program object

    // Triangle vertices (x, y, z coordinates for each vertex)
    // For a 2D triangle on the XY plane, Z is 0.
    // We define 3 vertices, each with 3 float components (x, y, z).
    float vertices[9] = {
        -0.5f, -0.5f, 0.0f, // Vertex 1: Bottom-left
         0.5f, -0.5f, 0.0f, // Vertex 2: Bottom-right
         0.0f,  0.5f, 0.0f  // Vertex 3: Top-center
    };
};

#endif // RENDERER_H
