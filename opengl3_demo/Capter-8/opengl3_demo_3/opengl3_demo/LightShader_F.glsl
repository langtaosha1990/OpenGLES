#version 300 es
precision mediump float;
in vec2 vTextCoor;
uniform sampler2D map0;
uniform sampler2D map1;
out vec4 FragColor;


void main()
{
    vec4 map0Color;
    vec4 map1Color;
    map0Color = texture(map0, vTextCoor);
    map1Color = texture(map1, vTextCoor);

    FragColor = map0Color * (map1Color + 0.25);
}
