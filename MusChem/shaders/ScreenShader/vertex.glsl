#version 330

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data, interpolated for each fragment
out vec2 UV;

// Values that stay constant for the whole mesh

void main()
{
    // Output position of the vertex, in clip space: MVP * position
    gl_Position = vec4(vertexPosition_modelspace, 1);

    // Color of each vertex to be interpolated
    UV = vertexUV;
}

