
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;    // 法向量
out vec3 v_normal; // 纹理坐标
void main()
{
    v_normal = a_normal;
    gl_Position =  a_position;
}
