#version 330 core
out vec4 FragColor;

uniform vec3 pointColor;

void main()
{
    FragColor = vec4(pointColor, 1.0);
}