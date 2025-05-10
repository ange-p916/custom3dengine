// Shader.cpp
// Implementation of the Shader class.

#include "MyFirstEngine/GameObject.h" 
#include "MyFirstEngine/Transform.h"
#include "MyFirstEngine/Renderer.h"
// etc.
#include <fstream>
#include <sstream>
#include <iostream>

// Constructor: reads shader source files, compiles and links them.
Shader::Shader(const char* vertexPath, const char* fragmentPath) : ID(0) {
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        std::cerr << "Vertex Path: " << vertexPath << std::endl;
        std::cerr << "Fragment Path: " << fragmentPath << std::endl;
        // ID remains 0 if shader loading fails
        return;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    unsigned int vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM"); // Check for linking errors

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // If ID is still 0 after all this (e.g., due to linking error),
    // it indicates a problem. checkCompileErrors will print details.
}

// Destructor
Shader::~Shader() {
    if (ID != 0) { // Only delete if a program was successfully created
        glDeleteProgram(ID);
    }
}

// Activates the shader program
void Shader::use() {
    if (ID != 0) { // Only use if a program was successfully created
        glUseProgram(ID);
    } else {
        // Optionally, you could log an error here if trying to use an invalid shader
        // std::cerr << "ERROR::SHADER::USE_FAILED Program ID is 0" << std::endl;
    }
}

// Utility uniform functions
void Shader::setBool(const std::string &name, bool value) const {
    if (ID != 0) glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
    if (ID != 0) glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
    if (ID != 0) glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Utility function for checking shader compilation/linking errors.
void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") { // It's a shader (Vertex or Fragment)
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else { // It's a shader program
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            // If linking failed, the program ID might be invalid, so we could reset it
            // This is a bit aggressive, but ensures ID is 0 if linking failed.
            // However, glCreateProgram() already returns 0 on failure.
            // The main check is that ID is non-zero before use.
        }
    }
}
