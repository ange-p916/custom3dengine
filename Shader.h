// Shader.h
// Declaration of the Shader class.
// Handles loading, compiling, and linking GLSL shaders.

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "glad/glad.h" // For GLuint, GLenum types

class Shader {
public:
    // The program ID
    unsigned int ID;

    // Constructor reads and builds the shader
    // Takes paths to vertex and fragment shader source files
    Shader(const char* vertexPath, const char* fragmentPath);

    // Destructor
    ~Shader();

    // Use/activate the shader program
    void use();

    // Utility uniform functions (you can add more as needed for different types)
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    // Future additions:
    // void setVec2(const std::string &name, float x, float y) const;
    // void setVec3(const std::string &name, float x, float y, float z) const;
    // void setMat4(const std::string &name, const float* matValue) const;


private:
    // Utility function for checking shader compilation/linking errors.
    // 'shader' is the shader or program ID.
    // 'type' is "VERTEX", "FRAGMENT", or "PROGRAM" for error messaging.
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif // SHADER_H
