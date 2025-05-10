// triangle.vert
// Vertex Shader with color attribute.

#version 330 core // Specify GLSL version 3.30, core profile

// Input vertex data
// Location 0: Position attribute
layout (location = 0) in vec3 aPos; 
// Location 1: Color attribute
layout (location = 1) in vec3 aColor; 

// Output variable to pass color to the fragment shader
// OpenGL will interpolate this value across the triangle's surface.
out vec3 vertexColor; 

void main()
{
    // Pass position to clip-space
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    
    // Pass the color attribute directly to the fragment shader
    vertexColor = aColor;
}
