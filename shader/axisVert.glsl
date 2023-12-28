#version 450 core

layout(location = 0) in vec3 iPosition;

uniform mat4 view;
uniform mat4 proj;
uniform vec4 color;

out vec4 vertFragCol;

void main(){
    gl_Position = proj * view * vec4(iPosition, 1.0f);
    vertFragCol = color;
}