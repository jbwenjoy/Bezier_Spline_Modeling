#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 trans;
uniform float scale;

void main()
{
    //float SCR_WIDTH = 1280;
    //float SCR_HEIGHT = 720;
    //vec2 trans_normalized = vec2((trans.x - SCR_WIDTH / 2) / (SCR_WIDTH / 2), (-trans.y + SCR_HEIGHT / 2) / (SCR_HEIGHT / 2));
    ////gl_Position = vec4(aPosXY_normalized * scale + trans, 0.0, 1.0);
    //gl_Position = vec4(aPos.xy * scale + trans_normalized, 0.0, 1.0);

    gl_Position = vec4(aPos.xy * scale + trans, 0.0, 1.0);

    //gl_Position = vec4(aPos * scale + trans.xy, 0.0, 1.0);
}
