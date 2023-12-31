#version 450 core

layout (location = 0) in vec3 vPosition;

uniform mat4 view;
uniform mat4 proj;

void main(){
    gl_Position = proj * view * vec4(vPosition, 1.f); 
}