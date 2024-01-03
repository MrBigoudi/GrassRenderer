#version 450 core

layout (points) in;
// layout (triangle_strip, max_vertices = 64) out;
layout (triangle_strip, max_vertices = 256) out;

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

out vec4 geomFragCol;
out vec3 geomFragNormal;
out vec3 geomFragPos;
out flat int geomIsNormal;


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
    out vec3 normals[NB_VERT_HIGH_LOD],
    out vec3 facingNormals[NB_VERT_HIGH_LOD],
    out vec3 bezierNormals[NB_VERT_HIGH_LOD]
    ){
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;

    float minWidth = width / 5.f;
    float widthDelta = minWidth / nbVert;
    float curWidth = width / 2.f;

    vec2 P0 = vec2(0.f);
    vec2 P1 = bend;
    vec2 P2 = vec2(tilt, height);

    vec3 facingNormal = vec3(-1.f, 0.f, 0.f);

    for(int i=0; i<nbVert-1; i+=2){
        float t = i / (1.f * nbVert);
        vec2 bendAndTilt = quadraticBezierCurve(t, P0, P1, P2);
        positions[i] = pos + vec3(-curWidth, bendAndTilt.y, bendAndTilt.x);
        positions[i+1] = pos + vec3(curWidth, bendAndTilt.y, bendAndTilt.x);
        curWidth -= widthDelta;

        vec2 bezierDerivative = quadraticBezierCurveDerivative(t, P0, P1, P2);
        vec3 bezierNormal = normalize(vec3(0.f, bezierDerivative.y, bezierDerivative.x));
        vec3 normal = cross(bezierNormal, facingNormal);
        // rotate the normals a bit
        normals[i] = normalize(getRotationMatrix(PI * -0.3f) * normal);
        normals[i+1] = normalize(getRotationMatrix(PI * 0.3f) * normal);

        // tmp
        bezierNormals[i] = bezierNormal;
        bezierNormals[i+1] = bezierNormal;
        facingNormals[i] = facingNormal;
        facingNormals[i+1] = facingNormal;
    }

    positions[nbVert-1] = pos + vec3(0.f, height, tilt);
    vec2 bezierDerivative = quadraticBezierCurveDerivative(1.f, P0, P1, P2);
    vec3 bezierNormal = normalize(vec3(0.f, bezierDerivative.y, bezierDerivative.x));
    normals[nbVert-1] = cross(bezierNormal, facingNormal);
    bezierNormals[nbVert-1] = bezierNormal;
    facingNormals[nbVert-1] = facingNormal;
}

vec4 getWorldPos(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], int vertex, float rotation){
    vec3 oldPosition = positions[vertex];
    vec3 curPosition = getRotationMatrix(rotation) * (oldPosition - center) + center;
    vec4 worldPosition = proj * view * vec4(curPosition, 1.f);
    return worldPosition;
}

void createTriangle(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], vec3 normals[NB_VERT_HIGH_LOD],
    float rotation, vec4 color, int indices[3]){
    gl_Position = getWorldPos(center, positions, indices[0], rotation);
    geomFragCol = color;
    geomFragNormal = normals[indices[0]];
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 0;
    EmitVertex();
    gl_Position = getWorldPos(center, positions, indices[1], rotation);
    geomFragCol = color;
    geomFragNormal = normals[indices[1]];
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 0;
    EmitVertex();
    gl_Position = getWorldPos(center, positions, indices[2], rotation);
    geomFragCol = color;
    geomFragNormal = normals[indices[2]];
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 0;
    EmitVertex();
    EndPrimitive();
}

void createTriangles(vec3 center, float rotation, vec4 color,
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


void drawRectangle(vec3 p0, vec3 p1, vec3 p2, vec3 p3, vec4 color) {
    // First triangle
    gl_Position = proj * view * vec4(p0, 1.0);
    geomFragCol = color;
    geomFragNormal = normalize(cross(p1 - p0, p2 - p0));
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 1;
    EmitVertex();

    gl_Position = proj * view * vec4(p1, 1.0);
    geomFragCol = color;
    geomFragNormal = normalize(cross(p1 - p0, p2 - p0));
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 1;
    EmitVertex();

    gl_Position = proj * view * vec4(p2, 1.0);
    geomFragCol = color;
    geomFragNormal = normalize(cross(p1 - p0, p2 - p0));
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 1;
    EmitVertex();

    EndPrimitive();

    // Second triangle
    gl_Position = proj * view * vec4(p0, 1.0);
    geomFragCol = color;
    geomFragNormal = normalize(cross(p2 - p0, p3 - p0));
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 1;
    EmitVertex();

    gl_Position = proj * view * vec4(p2, 1.0);
    geomFragCol = color;
    geomFragNormal = normalize(cross(p2 - p0, p3 - p0));
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 1;
    EmitVertex();

    gl_Position = proj * view * vec4(p3, 1.0);
    geomFragCol = color;
    geomFragNormal = normalize(cross(p2 - p0, p3 - p0));
    geomFragPos = gl_Position.xyz;
    geomIsNormal = 1;
    EmitVertex();

    EndPrimitive();
}

void drawArrow(vec3 start, vec3 end, vec4 color) {
    float width = 0.001f;
    vec3 dir = end * 0.1f;
    vec3 baseDownLeft = start - vec3(width, 0.f, 0.f);
    vec3 baseDownRight = start + vec3(width, 0.f, 0.f);
    vec3 baseTopLeft = baseDownLeft + dir;
    vec3 baseTopRight = baseDownRight + dir;
    // Draw the main rectangle
    drawRectangle(baseTopLeft, baseDownLeft, baseDownRight, baseTopRight, color);
}

void drawNormals(
    vec3 positions[NB_VERT_HIGH_LOD],
    vec3 normals[NB_VERT_HIGH_LOD],
    vec3 facingNormals[NB_VERT_HIGH_LOD],
    vec3 bezierNormals[NB_VERT_HIGH_LOD]
) {
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;
    for (int i = 0; i < nbVert; i++) {
        // Draw the normal arrow
        drawArrow(positions[i], normals[i], vec4(1.0, 1.0, 0.0, 1.0));  // Yellow color for normals
        // Draw the facing normal arrow
        drawArrow(positions[i], facingNormals[i], vec4(0.0, 0.0, 1.0, 1.0));  // Blue color for facing normals
        // Draw the bezier normal arrow
        drawArrow(positions[i], bezierNormals[i], vec4(1.0, 0.0, 0.0, 1.0));  // Red color for bezier normals
    }
}

void main(){
    vec3 pos = vertexData[0]._Position;
    float height = vertexData[0]._Height;
    float width = vertexData[0]._Width;
    vec4 color = vec4(vertexData[0]._Color, 1.f);
    float rotation = vertexData[0]._Rotation;
    float tilt = vertexData[0]._Tilt;
    vec2 bend = vertexData[0]._Bend;

    vec3 positions[NB_VERT_HIGH_LOD];
    vec3 normals[NB_VERT_HIGH_LOD];
    vec3 bezierNormals[NB_VERT_HIGH_LOD];
    vec3 facingNormals[NB_VERT_HIGH_LOD];
    getVerticesPositionsAndNormals(pos, width, height, tilt, bend, positions, normals, bezierNormals, facingNormals);
    createTriangles(pos, rotation, color, positions, normals);

    // tmp 
    drawNormals(positions, normals, bezierNormals, facingNormals);
}