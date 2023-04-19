#version 300 es
precision mediump float;
in vec2 vTextCoor;
uniform sampler2D map;
out vec4 outColor;
void main()
{
    outColor = texture(map, vTextCoor);
}
