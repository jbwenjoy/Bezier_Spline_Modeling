#version 330 core
layout (location = 0) in vec4 aPos;

void main()
{
    //float SCR_WIDTH = 1280;
    //float SCR_HEIGHT = 720;
    //gl_Position = vec4((aPos.x - SCR_WIDTH / 2) / (SCR_WIDTH / 2), (-aPos.y + SCR_HEIGHT / 2) / (SCR_HEIGHT / 2), 0.0, 1.0);
    gl_Position = aPos;
}
