#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 3) in mat4 instanceMatrix; // equal to 4 vecs: location = 3, 4, 5, 6

uniform mat4 projection;
uniform mat4 view;

void main()
{
    //gl_Position = transform[gl_InstanceID] * aPos;
    gl_Position = projection * view * instanceMatrix * aPos; 
}