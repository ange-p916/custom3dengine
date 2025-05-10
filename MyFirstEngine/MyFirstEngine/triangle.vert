// triangle.vert
// Basic Vertex Shader

#version 330 core // Specify GLSL version 3.30, core profile

// Input vertex data, different for all executions of this shader.
// 'layout (location = 0)' links this to the first vertex attribute (our positions)
// This corresponds to glVertexAttribPointer's first argument (index 0) in Renderer.cpp
layout (location = 0) in vec3 aPos; // The position variable has attribute position 0

void main()
{
    // gl_Position is a special output variable in GLSL that serves as the final
    // clip-space position of the vertex.
    // We pass the input position (aPos) directly.
    // It must be a vec4: (x, y, z, w). 'w' is used for perspective division.
    // For 2D rendering or simple cases, w can be 1.0.
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
