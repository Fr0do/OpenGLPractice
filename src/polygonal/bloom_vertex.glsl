#version 330 core
layout(location = 0) in vec2 vertex;
//layout(location = 1) in vec3 normal;
layout(location = 1) in vec2 texCoords;

out vec2 vTexCoords;

void main()
{
    gl_Position = vec4(vertex, 0.0,  1.0);
    vTexCoords = texCoords;
}