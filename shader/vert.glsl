#version 450 core
	
layout(binding = 0, std430) readonly buffer positions{
    vec3 iPosition[];  // Grass blade position
};

layout(binding = 1, std430) readonly buffer facings{
    vec2 iFacing[];    // Grass blade facing direction
};

layout(binding = 2, std430) readonly buffer heights{
    float iHeight[];   // Grass blade height
};

layout(binding = 3, std430) readonly buffer widths{
    float iWidth[];    // Grass blade width
};


out VertexData{
    vec3 _Position;
    vec2 _Facing;
    float _Height;
    float _Width;
} vertexData;


void main() {
    vertexData._Position = iPosition[gl_VertexID];
    vertexData._Facing = iFacing[gl_VertexID];
    vertexData._Height = iHeight[gl_VertexID];
    vertexData._Width = iWidth[gl_VertexID];
}