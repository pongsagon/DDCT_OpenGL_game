#version 330 core
in vec2 TexCoord; // Input from vertex shader
out vec4 color; // Output color

uniform sampler2D texture1; // UI texture

void main()
{
    color = texture(texture1, TexCoord); // Get color from the texture

}