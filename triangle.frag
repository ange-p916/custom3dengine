// triangle.frag
// Basic Fragment Shader

#version 330 core // Specify GLSL version 3.30, core profile

// Output data for the fragment shader
out vec4 FragColor; // This variable will hold the final color of the fragment

void main()
{
    // Set the fragment color to a solid orange.
    // Colors are in RGBA format, where each component ranges from 0.0 to 1.0.
    // R=1.0 (red), G=0.5 (green), B=0.2 (blue), A=1.0 (alpha/opacity)
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); // Orange color
}

