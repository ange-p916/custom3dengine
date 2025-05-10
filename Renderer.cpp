// Renderer.cpp
// Implementation of the Renderer class.

#include "Renderer.h"
#include "glad/glad.h" // For OpenGL functions (glGenVertexArrays, glBindBuffer, etc.)
#include <iostream>    // For std::cerr

// Constructor
Renderer::Renderer() : VAO(0), VBO(0), shaderProgram(nullptr) {
    // Initialize members to safe default values.
    // VAO and VBO are initialized to 0, indicating they are not yet generated.
    // shaderProgram is initialized to nullptr.
}

// Destructor
Renderer::~Renderer() {
    // Clean up OpenGL objects and the shader program to prevent resource leaks.
    
    // Delete the shader program if it was allocated
    if (shaderProgram) {
        delete shaderProgram;
        shaderProgram = nullptr; // Set to nullptr to avoid dangling pointer issues
    }

    // Delete the Vertex Buffer Object if it was generated
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0; // Reset to 0
    }

    // Delete the Vertex Array Object if it was generated
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0; // Reset to 0
    }
}

// Initializes shaders, buffers, and vertex array objects
bool Renderer::init() {
    // --- 1. Load Shaders ---
    // Create a new Shader object.
    // Paths are relative to the executable's working directory.
    // Ensure "triangle.vert" and "triangle.frag" are in the correct location.
    // (CMakeLists.txt should copy them to the build output directory).
    shaderProgram = new Shader("triangle.vert", "triangle.frag");
    
    // Check if the shader program was created and compiled/linked successfully.
    // The Shader class constructor sets ID to 0 if there's an error.
    if (!shaderProgram || shaderProgram->ID == 0) {
        std::cerr << "Failed to create or link shader program." << std::endl;
        // If shaderProgram was allocated but ID is 0, delete it to prevent memory leak
        if (shaderProgram) {
            delete shaderProgram;
            shaderProgram = nullptr;
        }
        return false; // Initialization failed
    }

    // --- 2. Set up Vertex Data and Buffers ---
    // Generate 1 Vertex Array Object (VAO) and store its ID in VAO
    glGenVertexArrays(1, &VAO);
    // Generate 1 Vertex Buffer Object (VBO) and store its ID in VBO
    glGenBuffers(1, &VBO);

    // Bind the VAO: all subsequent VBO operations and attribute configurations will be associated with this VAO.
    glBindVertexArray(VAO);

    // Bind the VBO to the GL_ARRAY_BUFFER target.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy the vertex data (defined in the 'vertices' array) into the VBO's memory.
    // sizeof(vertices): total size of the data in bytes.
    // vertices: pointer to the data.
    // GL_STATIC_DRAW: hint that the data will be modified once and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // --- 3. Configure Vertex Attributes ---
    // Tell OpenGL how to interpret the vertex data in the VBO.
    // For our triangle, we have one attribute: position.
    // Attribute location 0 (as specified in the vertex shader: layout (location = 0) in vec3 aPos;)
    // 3 components per vertex attribute (x, y, z).
    // GL_FLOAT: the type of the data.
    // GL_FALSE: data should not be normalized.
    // 3 * sizeof(float): stride - the byte offset between consecutive vertex attributes.
    // (void*)0: offset of the first component of the first attribute in the VBO.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable vertex attribute at location 0.

    // Unbind VBO (optional but good practice, as VAO now "knows" about this VBO and its configuration).
    // The VBO is still associated with the VAO.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Unbind VAO (very important to prevent accidental modification of this VAO's state).
    // Any subsequent VBO or attribute calls will not affect this VAO until it's rebound.
    glBindVertexArray(0);

    return true; // Initialization successful
}

// Renders the scene (currently just a triangle)
void Renderer::draw() {
    // --- Clear the screen ---
    // Set the clear color (RGBA). This is the color the screen will be cleared to.
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
    // Clear the color buffer.
    // We are not using a depth buffer yet, so only GL_COLOR_BUFFER_BIT.
    glClear(GL_COLOR_BUFFER_BIT);

    // --- Draw the triangle ---
    if (shaderProgram && shaderProgram->ID != 0) { // Ensure shader program is valid
        shaderProgram->use();       // Activate the shader program for rendering.
        glBindVertexArray(VAO);     // Bind the VAO (which contains our VBO and attribute configuration).
                                    // This makes the triangle's vertex data and attributes active.
        
        // Draw the triangle.
        // GL_TRIANGLES: render mode.
        // 0: starting index in the enabled arrays.
        // 3: number of vertices to render.
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glBindVertexArray(0);     // Unbind the VAO after drawing. Good practice.
    } else {
        std::cerr << "Renderer::draw() called with invalid shader program." << std::endl;
    }
}
