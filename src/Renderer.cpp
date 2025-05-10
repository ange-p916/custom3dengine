// Renderer.cpp
// Implementation of the Renderer class.
// Now handles vertices with position and color attributes.
#include "MyFirstEngine/GameObject.h" 
#include "MyFirstEngine/Transform.h"
#include "MyFirstEngine/Renderer.h"
// etc.
#include "glad/glad.h" // For OpenGL functions
#include <iostream>    // For std::cerr

// Constructor
Renderer::Renderer() : VAO(0), VBO(0), shaderProgram(nullptr) {
    // Initialize members to safe default values.
}

// Destructor
Renderer::~Renderer() {
    // Clean up OpenGL objects and the shader program.
    if (shaderProgram) {
        delete shaderProgram;
        shaderProgram = nullptr;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

// Initializes shaders, buffers, and vertex array objects
bool Renderer::init() {
    // --- 1. Load Shaders ---
    // Shader files will be updated to handle vertex colors.
    // Ensure "triangle.vert" and "triangle.frag" are updated and in the correct location.
   shaderProgram = new Shader("shaders/triangle.vert", "shaders/triangle.frag");
    
    if (!shaderProgram || shaderProgram->ID == 0) {
        std::cerr << "Failed to create or link shader program." << std::endl;
        if (shaderProgram) {
            delete shaderProgram;
            shaderProgram = nullptr;
        }
        return false;
    }

    // --- 2. Set up Vertex Data and Buffers ---
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Use the updated vertices array which now includes color data.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // --- 3. Configure Vertex Attributes ---
    // We now have two attributes: position and color.
    // The total size of one vertex's data is 6 floats (3 for position, 3 for color).
    // Stride = 6 * sizeof(float)

    // Position attribute (Attribute location 0)
    // Takes the first 3 floats of each vertex.
    // Offset is 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    // Color attribute (Attribute location 1)
    // Takes the next 3 floats of each vertex (after the position data).
    // Offset is 3 * sizeof(float) from the start of the vertex data.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    return true; // Initialization successful
}

// Renders the scene
void Renderer::draw() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);         

    if (shaderProgram && shaderProgram->ID != 0) {
        shaderProgram->use();       
        glBindVertexArray(VAO);     
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw 3 vertices
        glBindVertexArray(0);     
    } else {
        std::cerr << "Renderer::draw() called with invalid shader program." << std::endl;
    }
}
