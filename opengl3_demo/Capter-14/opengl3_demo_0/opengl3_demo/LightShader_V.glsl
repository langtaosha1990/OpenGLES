
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 textCoor;  //纹理坐标
layout(location = 2) in vec3 normal;    // 法向量
uniform mat4 u_mvpMatrix;   // 旋转矩阵
out vec2 vTextCoor; // 纹理坐标



void main()
{
    vTextCoor = textCoor;
    gl_Position = u_mvpMatrix * a_position;
}
