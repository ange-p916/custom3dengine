// triangle.frag
// Fragment Shader using interpolated vertex color.
// This shader receives the interpolated color from the vertex shader
// and sets the final color of the fragment (pixel).

#version 330 core // Specify GLSL version 3.30, core profile

// Input variable from the vertex shader (interpolated color)
// The 'in' keyword signifies that this variable receives its value from the
// corresponding 'out' variable in the vertex shader.
// The name 'vertexColor' must match the 'out' variable in triangle.vert.
in vec3 vertexColor; 

// Output data for the fragment shader
// FragColor is a built-in output variable (though often user-defined 'out vec4 outColor;')
// that determines the final color of the pixel being rendered.
out vec4 FragColor; 

void main()
{
    // Set the fragment's color to the interpolated color received from the vertex shader.
    // The alpha component is set to 1.0 (fully opaque).
    FragColor = vec4(vertexColor, 1.0f); 
}
