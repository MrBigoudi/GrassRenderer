#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 view;
uniform mat4 proj;

in VertexData {
    vec3 _Position;
    vec2 _Facing;
    float _Height;
    float _Width;
} vertexData[];

out vec4 geomFragCol;

const vec4 green = vec4(0.f, 1.f, 0.f, 1.f);

void main(){
    vec3 pos = vertexData[0]._Position;
    vec2 facing = vertexData[0]._Facing;
    float height = vertexData[0]._Height;
    float width = vertexData[0]._Width;


    float halfWidth = width / 2.f;
    // float halfWidth = 10.f;
    vec3 curPosition = vec3(0.f);
    vec4 worldPosition = vec4(0.f);

    // first vertex
    curPosition = pos - vec3(halfWidth, 0.f, 0.f);
    worldPosition = proj * view * vec4(curPosition, 1.f);
    gl_Position = worldPosition;
    geomFragCol = green;
    EmitVertex();

    // second vertex
    curPosition = pos + vec3(halfWidth, 0.f, 0.f);
    worldPosition = proj * view * vec4(curPosition, 1.f);
    gl_Position = worldPosition;
    geomFragCol = green;
    EmitVertex();

    // third vertex
    curPosition = pos + vec3(0.f, height, 0.f);
    worldPosition = proj * view * vec4(curPosition, 1.f);
    gl_Position = worldPosition;
    geomFragCol = green;
    EmitVertex();

    EndPrimitive();
}