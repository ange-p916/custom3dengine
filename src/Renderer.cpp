// Renderer.cpp
// Implementation of the Renderer class.
// Now handles vertices with position and color attributes,
// and uses a uniform to apply a position offset.
#include "MyFirstEngine/GameObject.h" 
#include "MyFirstEngine/Transform.h"
#include "MyFirstEngine/Renderer.h"
#include "glad/glad.h" 
#include <iostream>    

// Constructor
Renderer::Renderer() : VAO(0), VBO(0), shaderProgram(nullptr) {}

// Destructor
Renderer::~Renderer() {
    if (shaderProgram) {
        delete shaderProgram;
        shaderProgram = nullptr;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0; // Good practice to reset IDs after deletion
    }
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0; // Good practice to reset IDs after deletion
    }
}

// Initializes shaders, buffers, and vertex array objects
bool Renderer::init() {
    // Shader files should be "shaders/triangle.vert" and "shaders/triangle.frag"
    // if you are using the structured project with shaders in assets/shaders/
    // and your CMakeLists.txt copies them to a "shaders" subdirectory in the build output.
    // Ensure triangle.vert uses the uOffset uniform.
    shaderProgram = new Shader("shaders/triangle.vert", "shaders/triangle.frag"); 
    
    if (!shaderProgram || shaderProgram->ID == 0) {
        std::cerr << "Failed to create or link shader program." << std::endl;
        if (shaderProgram) { // Clean up if allocated but failed
            delete shaderProgram;
            shaderProgram = nullptr;
        }
        return false;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0)
    // Stride is 6 * sizeof(float) because each vertex has 3 pos floats + 3 color floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    // Color attribute (location 1)
    // Offset is 3 * sizeof(float) because color data starts after the 3 position floats
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    return true;
}

// Draw method now accepts a position
void Renderer::draw(const Vec3& position) { // <-- Ensure this parameter is present
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);         

    if (shaderProgram && shaderProgram->ID != 0) {
        shaderProgram->use();
        
        // Set the "uOffset" uniform in the shader
        // This uniform is declared in triangle.vert as "uniform vec3 uOffset;"
        GLint offsetLoc = glGetUniformLocation(shaderProgram->ID, "uOffset");
        if (offsetLoc != -1) { // Check if the uniform was found
            glUniform3f(offsetLoc, position.x, position.y, position.z);
        } else {
            // This warning can be helpful during development.
            // It means the shader isn't using "uOffset" or it's misspelled.
             std::cout << "Warning: Uniform 'uOffset' not found in shader." << std::endl;
        }
        
        glBindVertexArray(VAO);     
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw 3 vertices for the triangle
        glBindVertexArray(0);     
    } else {
        std::cerr << "Renderer::draw() called with invalid or uninitialized shader program." << std::endl;
    }
}
