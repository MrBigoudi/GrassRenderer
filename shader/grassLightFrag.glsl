#version 450 core

out vec4 oFragCol;

in vec2 TexCoords;

uniform sampler2D gAlbedoSpec;
uniform sampler2D gPosition;
uniform sampler2D gNormal;

void main(){
    oFragCol = texture(gAlbedoSpec, TexCoords);
}