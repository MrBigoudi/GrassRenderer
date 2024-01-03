#version 450 core

uniform vec3 sunCol;

out vec4 oCol;

void main(){
    oCol = vec4(sunCol, 1.f);
}