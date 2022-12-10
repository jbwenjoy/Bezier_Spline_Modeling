#version 330 core
in vec4 vertexColor;
out vec4 FragColor;
uniform vec3 color;

void main()
{
    FragColor = 0.5 * (vec4(color, 1.0) + vertexColor);
}