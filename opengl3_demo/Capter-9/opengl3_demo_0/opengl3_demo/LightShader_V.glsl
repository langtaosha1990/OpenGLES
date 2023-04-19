
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 textCoor;  //纹理坐标
out vec2 vTextCoor; // 纹理坐标
uniform float offset;


void main()
{
    gl_Position = a_position;
    gl_Position.x += offset;
    vTextCoor = textCoor;
}
