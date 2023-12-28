#version 450 core

out vec4 oFragCol;
in vec4 vertFragCol;
	
void main(){             
    oFragCol = vertFragCol;
}