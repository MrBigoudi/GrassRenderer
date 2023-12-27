#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 view;
uniform mat4 projection;

in vec3 geomPosition[];
in vec2 geomFacing[];
in float geomHeight[];
in float geomWidth[];

out vec4 geomFragCol;

const vec4 green = vec4(0., 1., 0., 1.);
const vec4 red = vec4(1., 0., 0., 1.);

void main(){
    float halfWidth = geomWidth[0] / 2.f;
    // float halfWidth = 10.f;
    vec3 curPosition = vec3(0.f);
    vec4 worldPosition = vec4(0.f);

    // first vertex
    curPosition = geomPosition[0] + vec3(-halfWidth, 0.f, 0.f);
    worldPosition = projection * view * vec4(curPosition, 1.f);
    gl_Position = worldPosition;
    geomFragCol = geomPosition[0].z != 0 ? green : red;
    EmitVertex();

    // second vertex
    curPosition = geomPosition[0] + vec3(halfWidth, 0.f, 0.f);
    worldPosition = projection * view * vec4(curPosition, 1.f);
    gl_Position = worldPosition;
    geomFragCol = geomPosition[0].z != 0 ? green : red;
    EmitVertex();

    // third vertex
    curPosition = geomPosition[0] + vec3(0.f, geomHeight[0], 0.f);
    worldPosition = projection * view * vec4(curPosition, 1.f);
    gl_Position = worldPosition;
    geomFragCol = geomPosition[0].z != 0 ? green : red;
    EmitVertex();

    EndPrimitive();
}