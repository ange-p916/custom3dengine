// triangle.vert
// Vertex Shader with MVP matrices for 3D camera.
// This shader takes vertex positions and colors, and transformation matrices
// (model, view, projection) to calculate the final screen position of each vertex.

#version 330 core // Specify GLSL version 3.30, core profile

// Input vertex attributes from the VBO
// layout (location = 0) links this to the first attribute pointer (positions)
layout (location = 0) in vec3 aPos;   // Vertex position in model space (local coordinates)
// layout (location = 1) links this to the second attribute pointer (colors)
layout (location = 1) in vec3 aColor; // Vertex color

// Uniforms: values passed from the C++ application to the shader
// These matrices transform the vertex positions.
uniform mat4 model;       // Model matrix: transforms from model space to world space
uniform mat4 view;        // View matrix: transforms from world space to view (camera) space
uniform mat4 projection;  // Projection matrix: transforms from view space to clip space (adds perspective)

// Output variable to pass color to the fragment shader
// The 'out' keyword means this variable's value will be interpolated
// for each fragment between the vertices.
out vec3 vertexColor;

void main()
{
    // Calculate the final position of the vertex in clip space.
    // The transformations are applied in reverse order of how they're thought of:
    // 1. Model: Transform the vertex from its local model space to world space.
    // 2. View: Transform the vertex from world space to the camera's view space.
    // 3. Projection: Transform the vertex from view space to clip space (ready for rasterization).
    // gl_Position is a special built-in variable that must be set by the vertex shader.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Pass the input color (aColor) to the fragment shader via vertexColor.
    // This color will be interpolated across the triangle's surface.
    vertexColor = aColor;
}
