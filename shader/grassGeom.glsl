#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 45) out;
// layout (triangle_strip, max_vertices = 256) out;

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
const float PI = 3.1416f;

in VertexData{
    vec3 _Position;
    float _Height;
    float _Width;
    vec3 _Color;
    float _Rotation;
    float _Tilt;
    vec2 _Bend;
} vertexData[];

out vec3 geomFragCol;
out vec3 geomFragNormal;
out vec3 geomFragPos;


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

void getVerticesPositionsAndNormals(vec3 pos, float width, float height, float tilt, vec2 bend,
    out vec3 positions[NB_VERT_HIGH_LOD],
    out vec3 normals[NB_VERT_HIGH_LOD]
    ){
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;

    float minWidth = width / 5.f;
    float widthDelta = minWidth / nbVert;
    float curWidth = width / 2.f;

    vec2 P0 = vec2(0.f);
    vec2 P1 = bend;
    vec2 P2 = vec2(tilt, height);

    vec3 widthTangent = vec3(0.f, 0.f, 1.f);

    for(int i=0; i<nbVert-1; i+=2){
        float t = i / (1.f * nbVert);
        vec2 bendAndTilt = quadraticBezierCurve(t, P0, P1, P2);
        positions[i] = pos + vec3(bendAndTilt.x, bendAndTilt.y, -curWidth);
        positions[i+1] = pos + vec3(bendAndTilt.x, bendAndTilt.y, curWidth);
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

    positions[nbVert-1] = pos + vec3(P2, 0.f);
    vec2 bezierDerivative = quadraticBezierCurveDerivative(1.f, P0, P1, P2);
    vec3 bezierNormal = normalize(vec3(bezierDerivative.x, bezierDerivative.y, 0.f));
    normals[nbVert-1] = cross(bezierNormal, widthTangent);
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


void createTriangle(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], vec3 normals[NB_VERT_HIGH_LOD], 
    float rotation, vec3 color, int indices[3]){
    int idx = 0;
    for(int i=0; i<3; i++){
        idx = indices[i];
        vec3 modelPos = getModelPos(center, positions, idx, rotation);
        vec4 worldPos = getWorldPos(center, positions, idx, rotation);
        geomFragCol = color;
        geomFragNormal = normals[idx];
        geomFragPos = modelPos;
        gl_Position = worldPos;
        EmitVertex();    
    }
    EndPrimitive();
}

void createTriangles(vec3 center, float rotation, vec3 color,
    vec3 positions[NB_VERT_HIGH_LOD], 
    vec3 normals[NB_VERT_HIGH_LOD]
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
        createTriangle(center, positions, normals, rotation, color, indices);

        // second triangle
        indices[0] = vertCounter+2;
        indices[1] = vertCounter+1;
        indices[2] = vertCounter+3;
        createTriangle(center, positions, normals, rotation, color, indices);

        vertCounter += 2;
    }
    // last triangle
    indices[0] = nbVert-3;
    indices[1] = nbVert-2;
    indices[2] = nbVert-1;
    createTriangle(center, positions, normals, rotation, color, indices);
}

void main(){
    vec3 pos = vertexData[0]._Position;
    float height = vertexData[0]._Height;
    float width = vertexData[0]._Width;
    vec3 color = vertexData[0]._Color;
    float rotation = vertexData[0]._Rotation;
    float tilt = vertexData[0]._Tilt;
    vec2 bend = vertexData[0]._Bend;

    vec3 positions[NB_VERT_HIGH_LOD];
    vec3 normals[NB_VERT_HIGH_LOD];
    getVerticesPositionsAndNormals(pos, width, height, tilt, bend, positions, normals);
    createTriangles(pos, rotation, color, positions, normals);

}