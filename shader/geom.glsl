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

in VertexData{
    vec3 _Position;
    float _Height;
    float _Width;
    vec3 _Color;
    float _Rotation;
    float _Tilt;
    vec2 _Bend;
} vertexData[];

out vec4 geomFragCol;


mat3 getRotationMatrix(float rotation){
    return mat3(cos(rotation), 0.f, sin(rotation),
                0.f, 1.f, 0.f,
                -sin(rotation), 0.f, cos(rotation));
}


vec2 quadraticBezierCurve(float t, vec2 P0, vec2 P1, vec2 P2){
    return (1-t)*(1-t)*P0 + 2*(1-t)*t*P1 + t*t*P2;
}

void getVerticesPositions(vec3 pos, float width, float height, float tilt, vec2 bend, out vec3 positions[NB_VERT_HIGH_LOD]){
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;

    float minWidth = width / 5.f;
    float widthDelta = minWidth / nbVert;
    float curWidth = width / 2;

    vec2 P0 = vec2(0.f);
    vec2 P1 = bend;
    vec2 P2 = vec2(tilt, height);

    for(int i=0; i<nbVert-1; i+=2){
        float t = i / (1.f * nbVert);
        vec2 bendAndTilt = quadraticBezierCurve(t, P0, P1, P2);
        positions[i] = pos + vec3(-curWidth, bendAndTilt.y, bendAndTilt.x);
        positions[i+1] = pos + vec3(curWidth, bendAndTilt.y, bendAndTilt.x);
        curWidth -= widthDelta;
    }

    positions[nbVert-1] = pos + vec3(0.f, height, tilt);
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
    float height = vertexData[0]._Height;
    float width = vertexData[0]._Width;
    vec4 color = vec4(vertexData[0]._Color, 1.f);
    float rotation = vertexData[0]._Rotation;
    float tilt = vertexData[0]._Tilt;
    vec2 bend = vertexData[0]._Bend;

    // float halfWidth = 10.f;
    vec3 positions[NB_VERT_HIGH_LOD];
    getVerticesPositions(pos, width, height, tilt, bend, positions);
    createTriangles(pos, positions, rotation, color);
}