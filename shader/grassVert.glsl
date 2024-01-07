#version 450 core
	
layout(binding = 0, std430) readonly buffer positions{
    vec4 iPosition[];  // Grass blade position
};

layout(binding = 1, std430) readonly buffer heights{
    float iHeight[];   // Grass blade height
};

layout(binding = 2, std430) readonly buffer widths{
    float iWidth[];    // Grass blade width
};

layout(binding = 3, std430) readonly buffer colors{
    vec4 iColor[];  // Grass blade color
};

layout(binding = 4, std430) readonly buffer rotations{
    float iRotation[];  // Grass blade rotation
};

layout(binding = 5, std430) readonly buffer tilts{
    float iTilt[];    // Grass blade tilt
};

layout(binding = 6, std430) readonly buffer bends{
    vec2 iBend[];    // Grass blade bend
};


out VertexData{
    vec4 _Position;
    float _Height;
    float _Width;
    vec4 _Color;
    float _Rotation;
    float _Tilt;
    vec2 _Bend;
} vertexData;


void main() {
    vertexData._Position = iPosition[gl_VertexID];
    vertexData._Height = iHeight[gl_VertexID];
    vertexData._Width = iWidth[gl_VertexID];
    vertexData._Color = iColor[gl_VertexID];
    vertexData._Rotation = iRotation[gl_VertexID];
    vertexData._Tilt = iTilt[gl_VertexID];
    vertexData._Bend = iBend[gl_VertexID];
}