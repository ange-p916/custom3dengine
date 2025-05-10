// Renderer.h
// Renderer class declaration.
// Responsible for initializing OpenGL rendering state, managing shader programs,
// vertex buffers, and drawing objects.
// Now updated to accept Model, View, and Projection matrices for 3D rendering.

#ifndef RENDERER_H
#define RENDERER_H

#include "MyFirstEngine/Shader.h" // Path to Shader.h, assuming it's in include/MyFirstEngine/
#include "../SimpleMath.h"        // Path to SimpleMath.h for Mat4 and Vec3 definitions,
                                  // assuming Renderer.h is in include/MyFirstEngine/
                                  // and SimpleMath.h is in the parent include/ directory.

class Renderer {
public:
    // Constructor
    Renderer();
    // Destructor: cleans up OpenGL resources (VAO, VBO, shader program)
    ~Renderer();

    // Initializes the renderer:
    // - Loads and compiles shaders.
    // - Sets up Vertex Array Object (VAO) and Vertex Buffer Object (VBO) for a triangle.
    // - Configures vertex attributes (position and color).
    // - Enables depth testing for 3D.
    // Returns true on successful initialization, false otherwise.
    bool init();

    // Draws the scene (currently a single triangle).
    // model: The model matrix for the object being drawn (transforms from model to world space).
    // view: The view matrix (transforms from world to camera/view space).
    // projection: The projection matrix (transforms from camera/view to clip space, adds perspective).
    void draw(const Mat4& model, const Mat4& view, const Mat4& projection);

private:
    unsigned int VAO;         // Vertex Array Object ID
    unsigned int VBO;         // Vertex Buffer Object ID
    Shader* shaderProgram;    // Pointer to the Shader object managing our GLSL program

    // Vertex data for a triangle. Each vertex has 3 position floats (x,y,z)
    // and 3 color floats (r,g,b). Total 6 floats per vertex.
    // Stored as: PosX, PosY, PosZ, ColR, ColG, ColB, ...
    float vertices[18] = {
        // Positions          // Colors
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Vertex 1: Bottom-left (Red)
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // Vertex 2: Bottom-right (Green)
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f  // Vertex 3: Top-center (Blue)
    };
    // Note: For a 3D scene, you might want to define vertices with non-zero Z values
    // or load them from a model file. This triangle lies on the Z=0 plane.
};

#endif // RENDERER_H
