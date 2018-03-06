#version 330

// Ouput data
out vec4 color;

uniform vec3 provColor;

void main()
{
    color = vec4(provColor.xyz, 1.0);
}
