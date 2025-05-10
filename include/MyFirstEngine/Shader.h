// Shader.h
// Declaration of the Shader class.
// Handles loading, compiling, and linking GLSL shaders from files.
// Includes methods for setting various uniform types, including 4x4 matrices.

#ifndef SHADER_H
#define SHADER_H

#include <string>      // For std::string (used for uniform names and file paths)
#include "glad/glad.h" // For OpenGL types (GLuint, GLenum, etc.)
                       // GLAD must be included before any other OpenGL headers if not already handled.

// The Shader class encapsulates the creation and management of an OpenGL shader program.
class Shader {
public:
    // The program ID of the linked shader program.
    // This ID is used when activating the shader or setting uniforms.
    // It will be 0 if shader creation or linking failed.
    unsigned int ID;

    // Constructor:
    // Reads GLSL shader source code from the specified vertex and fragment shader files.
    // Compiles and links the shaders into a shader program.
    // Stores the program ID in the 'ID' member.
    // vertexPath: file path to the vertex shader source code.
    // fragmentPath: file path to the fragment shader source code.
    Shader(const char* vertexPath, const char* fragmentPath);

    // Destructor:
    // Cleans up by deleting the shader program from OpenGL if it was created.
    ~Shader();

    // Activates the shader program for use in rendering.
    // This is equivalent to calling glUseProgram(ID).
    void use();

    // Utility functions for setting uniform values in the shader program.
    // These functions find the uniform location by name and then set its value.
    // They should be called after 'use()' has been called for this shader program.

    // Sets a boolean uniform.
    void setBool(const std::string &name, bool value) const;
    // Sets an integer uniform.
    void setInt(const std::string &name, int value) const;
    // Sets a float uniform.
    void setFloat(const std::string &name, float value) const;
    // Sets a 4x4 matrix uniform (e.g., model, view, projection matrices).
    // matValue: a pointer to the first element of a 16-float array representing the matrix
    //           (expected to be in column-major order, as used by OpenGL and our Mat4).
    void setMat4(const std::string &name, const float* matValue) const;
    // You can add more setters for other uniform types (vec2, vec3, vec4, mat2, mat3, etc.) as needed.

private:
    // A private utility function to check for compilation or linking errors.
    // shader: the ID of the shader or program to check.
    // type: a string indicating whether it's a "VERTEX" shader, "FRAGMENT" shader, or "PROGRAM".
    //       This is used for printing more informative error messages.
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif // SHADER_H
