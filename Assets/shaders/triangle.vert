// triangle.vert
// Vertex Shader with color attribute and position offset uniform.

#version 330 core // Specify GLSL version 3.30, core profile

// Input vertex data
// Location 0: Position attribute
layout (location = 0) in vec3 aPos; 
// Location 1: Color attribute
layout (location = 1) in vec3 aColor; 

// Uniform for offsetting the position of the entire object
// This value will be set from your C++ code (Renderer.cpp)
uniform vec3 uOffset; 

// Output variable to pass color to the fragment shader
out vec3 vertexColor; 

void main()
{
    // Add the uOffset to the original vertex position (aPos)
    vec3 finalPosition = aPos + uOffset;
    
    // Set the final clip-space position
    gl_Position = vec4(finalPosition, 1.0);
    
    // Pass the color attribute directly to the fragment shader
    vertexColor = aColor;
}
