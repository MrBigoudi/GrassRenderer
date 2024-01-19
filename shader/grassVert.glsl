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

// uniform int parallelId;
// uniform int nbBladesPerTile;
uniform int startId;

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
    int id = gl_VertexID;
    id += startId;

    vertexData._Position = iPosition[id];
    vertexData._Height = iHeight[id];
    vertexData._Width = iWidth[id];
    vertexData._Color = iColor[id];
    vertexData._Rotation = iRotation[id];
    vertexData._Tilt = iTilt[id];
    vertexData._Bend = iBend[id];
}