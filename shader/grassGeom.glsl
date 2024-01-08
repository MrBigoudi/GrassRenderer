#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 45) out;
// layout (triangle_strip, max_vertices = 256) out;

uniform mat4 view;
uniform mat4 proj;
uniform int tileLOD;
uniform float time;

const int HIGH_LOD = 1;
const int NB_VERT_HIGH_LOD = 15;
const int NB_QUAD_HIGH_LOD = 6;

const int LOW_LOD = 2;
const int NB_VERT_LOW_LOD = 3;
const int NB_QUAD_LOW_LOD = 0;

const vec3 TIP_COLOR = vec3(0.5f, 0.5f, 0.1f);
const vec4 red = vec4(1.f, 0.f, 0.f, 1.f);
const float PI = 3.1416f;

in VertexData{
    vec4 _Position;
    float _Height;
    float _Width;
    vec4 _Color;
    float _Rotation;
    float _Tilt;
    vec2 _Bend;
} vertexData[];

out vec3 geomFragCol;
out vec3 geomFragNormal;
out vec3 geomFragPos;

uniform vec3 camPos;
uniform vec3 camAt;
uniform int tileWidth;
uniform int tileHeight;

const int perlinNoiseGridSize = 4;
vec2 perlinNoiseGrid[perlinNoiseGridSize*perlinNoiseGridSize] = {
    vec2(0.771012, 0.636019),
    vec2(0.160682, 0.987007),
    vec2(0.359264, 0.933235),
    vec2(0.880301, 0.474678),
    vec2(0.678273, 0.734603),
    vec2(0.205294, 0.978705),
    vec2(0.838856, 0.544077),
    vec2(0.949798, 0.312973),
    vec2(0.594761, 0.803876),
    vec2(0.712568, 0.701568),
    vec2(0.436498, 0.899715),
    vec2(0.999037, 0.044264),
    vec2(0.520423, 0.853941),
    vec2(0.432170, 0.901753),
    vec2(0.963676, 0.266713),
    vec2(0.781059, 0.624056)
};

vec2 getPerlinNoiseGradient(int u, int v){
    int id = u + v*perlinNoiseGridSize;
    return perlinNoiseGrid[id];
}

vec2 getGridCell(vec2 pos, float cellWidth, float cellHeight){
    int cellX = int(pos.x / cellWidth);
    int cellY = int(pos.y / cellHeight);

    return vec2(cellX, cellY);
}

float blending(float t){
    return 6.f*t*t*t*t*t - 15.f*t*t*t*t + 10.f*t*t*t;
}

float perlin(vec2 pos){
    float cellWidth = (1.f*tileWidth) / perlinNoiseGridSize;
    float cellHeight = (1.f*tileHeight) / perlinNoiseGridSize;

    vec2 ij = getGridCell(pos, cellWidth, cellHeight);

    float x = pos.x;
    float y = pos.y;

    int i = int(ij.x);
    int j = int(ij.y);

    vec2 g00 = getPerlinNoiseGradient(i, j);
    vec2 g10 = getPerlinNoiseGradient(i+1, j);
    vec2 g01 = getPerlinNoiseGradient(i, j+1);
    vec2 g11 = getPerlinNoiseGradient(i+1, j+1);

    float u = x-i*cellWidth;
    float v = y-i*cellHeight;

    float n00 = dot(g00, vec2(u,v));
    float n10 = dot(g10, vec2(u-1.f,v));
    float n01 = dot(g01, vec2(u,v-1.f));
    float n11 = dot(g11, vec2(u-1.f,v-1.f));

    float nx0 = n00*(1-blending(u)) + n10*blending(u);
    float nx1 = n01*(1-blending(u)) + n11*blending(u);
    float nxy = nx0*(1-blending(v)) + nx1*blending(v);

    return nxy;
}

float infinitePerlin(vec2 pos){
    // vec2 wrappedP = mod(pos, perlinNoiseGridSize);
    vec2 wrappedP = mod(pos, perlinNoiseGridSize);
    return perlin(wrappedP + vec2(time * 0.35f));
}

mat3 getRotationMatrix(float rotation){
    return mat3(cos(rotation), 0.f, sin(rotation),
                0.f, 1.f, 0.f,
                -sin(rotation), 0.f, cos(rotation));
}


vec2 quadraticBezierCurve(float t, vec2 P0, vec2 P1, vec2 P2){
    return (1.f-t)*(1.f-t)*P0 + 2.f*(1.f-t)*t*P1 + t*t*P2;
}

vec2 quadraticBezierCurveDerivative(float t, vec2 P0, vec2 P1, vec2 P2){
    return -2.f*(1.f-t)*P0 + 2.f*P1*(1.f-2.f*t) + 2.f*t*P2;
}

vec3 getColor(vec3 color, float maxHeight, float curHeight){
    return mix(color, TIP_COLOR, (curHeight / maxHeight) * 0.8f);
}

void getVerticesPositionsAndNormals(vec3 pos, float width, float height, float tilt, vec2 bend, vec3 color,
    out vec3 positions[NB_VERT_HIGH_LOD],
    out vec3 normals[NB_VERT_HIGH_LOD],
    out vec3 colors[NB_VERT_HIGH_LOD]
    ){
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;

    float minWidth = width / 5.f;
    float widthDelta = minWidth / nbVert;
    float curWidth = width / 2.f;

    float noise = infinitePerlin(pos.xz);

    vec2 P0 = vec2(0.f);
    vec2 P1 = bend;
    vec2 P2 = vec2(tilt, height);
    // vec2 P2 = vec2(tilt, height) + vec2(0.1f, 0.3f) * noise;

    vec3 widthTangent = vec3(0.f, 0.f, 1.f);

    for(int i=0; i<nbVert-1; i+=2){
        float t = i / (1.f * nbVert);
        vec2 bendAndTilt = quadraticBezierCurve(t, P0, P1, P2);
        positions[i] = pos + vec3(bendAndTilt.x, bendAndTilt.y, -curWidth);
        positions[i+1] = pos + vec3(bendAndTilt.x, bendAndTilt.y, curWidth);
        // colors[i] = getColor(color, height, bendAndTilt.y);
        // colors[i+1] = getColor(color, height, bendAndTilt.y);
        colors[i] = noise*vec3(1.f,1.f,1.f);
        colors[i+1] = noise*vec3(1.f,1.f,1.f);
        curWidth -= widthDelta;

        vec2 bezierDerivative = quadraticBezierCurveDerivative(t, P0, P1, P2);
        vec3 bezierNormal = normalize(vec3(bezierDerivative.x, bezierDerivative.y, 0.f));
        vec3 normal = cross(bezierNormal, widthTangent);
        // rotate the normals a bit
        // normals[i] = normal;
        // normals[i+1] = normal;
        normals[i] = normalize(getRotationMatrix(PI * 0.3f) * normal);
        normals[i+1] = normalize(getRotationMatrix(PI * (-0.3f)) * normal);
    }

    vec3 newPos = pos + vec3(P2, 0.f);
    positions[nbVert-1] = newPos;
    vec2 bezierDerivative = quadraticBezierCurveDerivative(1.f, P0, P1, P2);
    vec3 bezierNormal = normalize(vec3(bezierDerivative.x, bezierDerivative.y, 0.f));
    normals[nbVert-1] = cross(bezierNormal, widthTangent);
    colors[nbVert-1] = TIP_COLOR;
    colors[nbVert-1] = noise*vec3(1.f,1.f,1.f);
}

vec3 getModelPos(vec3 center, vec3 position, float rotation){
    return getRotationMatrix(rotation) * (position - center) + center;
}

vec3 getModelPos(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], int vertex, float rotation){
    return getModelPos(center, positions[vertex], rotation);
}

vec4 getWorldPos(vec3 center, vec3 position, float rotation){
    vec3 curPosition = getModelPos(center, position, rotation);
    vec4 worldPosition = proj * view * vec4(curPosition, 1.f);
    return worldPosition;
}

vec4 getWorldPos(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], int vertex, float rotation){
    return getWorldPos(center, positions[vertex], rotation);
}

vec3 getRotatedNormals(vec3 normal, float rotation){
    return normalize(getRotationMatrix(rotation) * normal);
}

void createTriangle(vec3 center, 
    vec3 positions[NB_VERT_HIGH_LOD], 
    vec3 normals[NB_VERT_HIGH_LOD], 
    vec3 colors[NB_VERT_HIGH_LOD],
    float rotation, vec3 color, int indices[3]){
    int idx = 0;
    for(int i=0; i<3; i++){
        idx = indices[i];
        vec3 modelPos = getModelPos(center, positions, idx, rotation);
        vec4 worldPos = getWorldPos(center, positions, idx, rotation);
        vec3 normal = getRotatedNormals(normals[idx], rotation);
        geomFragCol = colors[idx];
        geomFragNormal = normal;
        geomFragPos = modelPos;
        gl_Position = worldPos;
        EmitVertex();    
    }
    EndPrimitive();
}

void createTriangles(vec3 center, float rotation, vec3 color,
    vec3 positions[NB_VERT_HIGH_LOD], 
    vec3 normals[NB_VERT_HIGH_LOD],
    vec3 colors[NB_VERT_HIGH_LOD]
    ){
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
        createTriangle(center, positions, normals, colors, rotation, color, indices);

        // second triangle
        indices[0] = vertCounter+2;
        indices[1] = vertCounter+1;
        indices[2] = vertCounter+3;
        createTriangle(center, positions, normals, colors, rotation, color, indices);

        vertCounter += 2;
    }
    // last triangle
    indices[0] = nbVert-3;
    indices[1] = nbVert-2;
    indices[2] = nbVert-1;
    createTriangle(center, positions, normals, colors, rotation, color, indices);
}

vec3 getAvgNormal(float tilt, float height){
    vec3 P0 = vec3(0.f);
    vec3 P1 = vec3(tilt, height, 0.f);
    vec3 widthTangent = vec3(0.f, 0.f, 1.f);
    vec3 heightTangent = normalize(P1-P0);
    return cross(heightTangent, widthTangent);
}

float getRotation(float tilt, float height){
    float rotation = vertexData[0]._Rotation;
    vec3 normal = getAvgNormal(tilt, height);
    normal = (view*vec4(getRotatedNormals(normal, rotation), 1.f)).xyz;

    vec3 camDir = vec3(0.f, 0.f, -1.f);
    float dotProduct = dot(normalize(camDir), normalize(normal));
    rotation += acos(dotProduct) / 3.f;

    return rotation;
}

void main(){
    vec3 pos = vertexData[0]._Position.xyz;
    float height = vertexData[0]._Height;
    float width = vertexData[0]._Width;
    vec3 color = vertexData[0]._Color.xyz;
    float tilt = vertexData[0]._Tilt;
    vec2 bend = vertexData[0]._Bend;
    float rotation = getRotation(tilt, height);

    vec3 positions[NB_VERT_HIGH_LOD];
    vec3 normals[NB_VERT_HIGH_LOD];
    vec3 colors[NB_VERT_HIGH_LOD];
    getVerticesPositionsAndNormals(pos, width, height, tilt, bend, color, positions, normals, colors);
    createTriangles(pos, rotation, color, positions, normals, colors);

}