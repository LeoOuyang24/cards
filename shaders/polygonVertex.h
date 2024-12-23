#version 330 core

layout (location = 0) in vec2 points;
layout (location = 1) in int depth;
layout (location = 2) in vec4 color;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec4 shade;

void main()
{
    gl_Position = vec4(points,(projection*view*vec4(points,depth,1)).z,1);
    shade = color;
}
