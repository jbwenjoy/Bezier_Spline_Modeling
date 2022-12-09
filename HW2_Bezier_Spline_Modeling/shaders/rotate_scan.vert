#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * aPos;
    //TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}