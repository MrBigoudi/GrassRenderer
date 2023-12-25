#version 330 core

in vec4 fCol;
in vec3 fNorm;
in vec3 fPos;
in vec2 fUvs;

out vec4 color;

/**
 * The Phong model
*/
void main(){
    color = fCol;
}