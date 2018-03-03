#version 330

// Interpolated values from the vertex shaders

// Ouput data
out vec4 color;

uniform vec3 providedColor;

void main()
{
    color = vec4(providedColor.xyz, 1.0);
}
