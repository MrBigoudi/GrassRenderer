#version 450 core

out vec4 oFragCol;
in vec4 geomFragCol;
	
void main()
{             
    oFragCol = geomFragCol;
}