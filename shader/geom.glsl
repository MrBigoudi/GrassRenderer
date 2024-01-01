#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 45) out;

uniform mat4 view;
uniform mat4 proj;
uniform int tileLOD;

const int HIGH_LOD = 1;
const int LOW_LOD = 2;
const int NB_VERT_HIGH_LOD = 15;
const int NB_VERT_LOW_LOD = 7;
const int NB_QUAD_HIGH_LOD = 6;
const int NB_QUAD_LOW_LOD = 2;

const vec4 red = vec4(1.f, 0.f, 0.f, 1.f);

in VertexData {
    vec3 _Position;
    vec2 _Facing;
    float _Height;
    float _Width;
    vec3 _Color;
    float _Rotation;
} vertexData[];

out vec4 geomFragCol;


mat3 getRotationMatrix(float rotation){
    return mat3(cos(rotation), 0.f, sin(rotation),
                0.f, 1.f, 0.f,
                -sin(rotation), 0.f, cos(rotation));
}


void getVerticesPositions(vec3 pos, float halfWidth, float height, out vec3 positions[NB_VERT_HIGH_LOD]){
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;
    float heightDelta = height / (nbVert / 2);
    float curHeight = 0.f;
    for(int i=0; i<nbVert-1; i+=2){
        positions[i] = pos + vec3(-halfWidth, curHeight, 0.f);
        positions[i+1] = pos + vec3(halfWidth, curHeight, 0.f);
        curHeight += heightDelta;
    }

    positions[nbVert-1] = pos + vec3(0.f, height, 0.f);
}

vec4 getWorldPos(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], int vertex, float rotation){
    vec3 oldPosition = positions[vertex];
    vec3 curPosition = getRotationMatrix(rotation) * (oldPosition - center) + center;
    vec4 worldPosition = proj * view * vec4(curPosition, 1.f);
    return worldPosition;
}

void createTriangle(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], float rotation, vec4 color, int indices[3]){
    gl_Position = getWorldPos(center, positions, indices[0], rotation);
    geomFragCol = color;
    EmitVertex();
    gl_Position = getWorldPos(center, positions, indices[1], rotation);
    geomFragCol = color;
    EmitVertex();
    gl_Position = getWorldPos(center, positions, indices[2], rotation);
    geomFragCol = color;
    EmitVertex();
    EndPrimitive();
}

void createTriangles(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], float rotation, vec4 color){
    int nbQuads = tileLOD == HIGH_LOD ? NB_QUAD_HIGH_LOD : NB_QUAD_LOW_LOD;
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;
    int vertCounter = 0;
    int indices[3];
    // draw the rectangles
    for(int i=0; i<nbQuads; i++){
        // first triangle
        indices[0] = vertCounter;
        indices[1] = vertCounter+1;
        indices[2] = vertCounter+2;
        createTriangle(center, positions, rotation, color, indices);

        // second triangle
        indices[0] = vertCounter+2;
        indices[1] = vertCounter+1;
        indices[2] = vertCounter+3;
        createTriangle(center, positions, rotation, color, indices);

        vertCounter += 2;
    }
    // last triangle
    indices[0] = nbVert-3;
    indices[1] = nbVert-2;
    indices[2] = nbVert-1;
    createTriangle(center, positions, rotation, color, indices);
}


void main(){
    vec3 pos = vertexData[0]._Position;
    vec2 facing = vertexData[0]._Facing;
    float height = vertexData[0]._Height;
    float width = vertexData[0]._Width;
    vec4 color = vec4(vertexData[0]._Color, 1.f);
    float rotation = vertexData[0]._Rotation;

    float halfWidth = width / 2.f;
    // float halfWidth = 10.f;
    vec3 positions[NB_VERT_HIGH_LOD];
    getVerticesPositions(pos, halfWidth, height, positions);
    createTriangles(pos, positions, rotation, color);
}