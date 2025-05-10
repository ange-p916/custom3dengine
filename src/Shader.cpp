// Shader.cpp
// Implementation of the Shader class.
// Handles loading shader source from files, compiling, linking,
// and providing utility functions for setting uniforms.

#include "MyFirstEngine/Shader.h" // Path to Shader.h, assuming Shader.h is in include/MyFirstEngine/
#include <fstream>               // For std::ifstream (file input stream)
#include <sstream>               // For std::stringstream (string stream for reading file buffer)
#include <iostream>              // For std::cerr (error output)

// Constructor: Reads shader source files, compiles and links them.
Shader::Shader(const char* vertexPath, const char* fragmentPath) : ID(0) {
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions on failure (badbit) or logical error (failbit)
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
        std::cerr << "Vertex Path: " << vertexPath << "; Fragment Path: " << fragmentPath << std::endl;
        // ID remains 0 if shader loading fails, indicating an error.
        return;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    unsigned int vertexShader, fragmentShader;

    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX"); // Check for compilation errors

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT"); // Check for compilation errors

    // Shader Program: Link the compiled shaders
    ID = glCreateProgram(); // Create a shader program and get its ID
    glAttachShader(ID, vertexShader);   // Attach the vertex shader
    glAttachShader(ID, fragmentShader); // Attach the fragment shader
    glLinkProgram(ID);                 // Link the program
    checkCompileErrors(ID, "PROGRAM"); // Check for linking errors

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // If ID is still 0 after all this (e.g., due to linking error),
    // it indicates a problem. checkCompileErrors will print details.
}

// Destructor: Cleans up the shader program
Shader::~Shader() {
    if (ID != 0) { // Only delete if a program was successfully created and linked
        glDeleteProgram(ID);
    }
}

// Activates the shader program
void Shader::use() {
    if (ID != 0) { // Only use if the program ID is valid
        glUseProgram(ID);
    } else {
        // Optionally, log an error if trying to use an invalid shader program
        // std::cerr << "ERROR::SHADER::USE_FAILED Program ID is 0. Shader might not have compiled/linked correctly." << std::endl;
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

// Sets a 4x4 matrix uniform in the shader program
void Shader::setMat4(const std::string &name, const float* matValue) const {
    if (ID != 0) {
        // glGetUniformLocation gets the location of the uniform variable 'name' in the shader program 'ID'.
        // glUniformMatrix4fv sets the value of the 4x4 float matrix uniform.
        // 1: number of matrices to modify (we are modifying 1 matrix).
        // GL_FALSE: specifies whether the matrix should be transposed.
        //           Our Mat4 class stores data in column-major order, which OpenGL expects, so no transpose is needed.
        // matValue: a pointer to the first element of the 16-float array representing the matrix.
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) { // Check if uniform was actually found
             glUniformMatrix4fv(loc, 1, GL_FALSE, matValue);
        } else {
            // This can be a common source of bugs if uniform names don't match
            // or if the uniform is optimized out by the shader compiler because it's not used.
            // std::cerr << "Warning: Uniform '" << name << "' not found in shader program ID " << ID << std::endl;
        }
    }
}

// Utility function for checking shader compilation and linking errors.
void Shader::checkCompileErrors(GLuint shaderOrProgramID, std::string type) {
    GLint success;
    GLchar infoLog[1024]; // Buffer for storing the error message
    if (type != "PROGRAM") { // It's a shader (Vertex or Fragment)
        glGetShaderiv(shaderOrProgramID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderOrProgramID, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else { // It's a shader program (checking for linking errors)
        glGetProgramiv(shaderOrProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderOrProgramID, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            // If linking failed, the program ID (this->ID) might be invalid.
            // The constructor should ideally handle this by not setting ID or setting it to 0.
        }
    }
}
