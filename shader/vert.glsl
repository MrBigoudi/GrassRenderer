#version 450 core
	
layout(location = 0) in vec3 iPosition;  // Grass blade position
layout(location = 1) in vec2 iFacing;    // Grass blade facing direction
layout(location = 2) in float iHeight;   // Grass blade height
layout(location = 3) in float iWidth;    // Grass blade width

out vec3 geomPosition;
out vec2 geomFacing;
out float geomHeight;
out float geomWidth;

void main() {
    geomPosition = iPosition;
    geomFacing = iFacing;
    geomHeight = iHeight;
    geomWidth = iWidth;
}