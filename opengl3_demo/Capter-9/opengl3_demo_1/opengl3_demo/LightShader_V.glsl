
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;  //纹理坐标
out vec2 vTextCoor; // 纹理坐标

void main()
{
    vTextCoor = a_texCoord;
    gl_Position = a_position;
}
