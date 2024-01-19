#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 45) out;
// layout (triangle_strip, max_vertices = 256) out;

uniform mat4 view;
uniform mat4 proj;
uniform int tileLOD;

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

uniform float time;

/* Gradient Perlin noise

uniform int tileWidth;
uniform int tileHeight;

const int perlinNoiseGridSize = 4;

float rand(vec2 co){
    return fract(sin(dot(co ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 getRandomGradient(int id){
    float x = rand(vec2(1.f*id, 0.f));
    float y = rand(vec2(0.f, 1.f*id));
    return vec2(x, y);
}

vec2 getPerlinNoiseGradient(int u, int v){
    int id = u + v*perlinNoiseGridSize;
    return getRandomGradient(id);
}

vec2 getGridCell(vec2 pos, float cellWidth, float cellHeight){
    int cellX = int(floor(pos.x / cellWidth));
    int cellY = int(floor(pos.y / cellHeight));
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
    float v = y-j*cellHeight;

    float n00 = dot(g00, vec2(u,v));
    float n10 = dot(g10, vec2(u-1.f,v));
    float n01 = dot(g01, vec2(u,v-1.f));
    float n11 = dot(g11, vec2(u-1.f,v-1.f));

    float nx0 = n00*(1-blending(u)) + n10*blending(u);
    float nx1 = n01*(1-blending(u)) + n11*blending(u);
    float nxy = nx0*(1-blending(v)) + nx1*blending(v);


    return nxy * 0.5f + 0.5f; // [0,1]
    // return nxy; // [-1,1]
}

float infinitePerlin(vec2 pos){
    vec2 wrappedP = mod(pos, perlinNoiseGridSize);
    return perlin(wrappedP + vec2(cos(0.50f*time), sin(time)*0.35f));
}
*/

// ************************************************ //
// GLSL Simplex Noise
// //
// Description : Array and textureless GLSL 2D/3D/4D simplex
// noise functions.
// Author : Ian McEwan, Ashima Arts.
// Maintainer : ijm
// Lastmod : 20110822 (ijm)
// License : Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise
//
// See: https://www.shadertoy.com/view/Mds3Wr
// ************************************************ //
float smooth_snoise(in vec2 v);
float smooth_snoise(in vec3 v);
float snoise(in vec2 v, int octaves) {
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += smooth_snoise(v) * scale;
		v *= vec2(2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}

float snoise(in vec3 v, int octaves) {
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += smooth_snoise(v) * scale;
		v *= vec3(2.0, 2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}

float snoise_abs(in vec2 v, int octaves) {
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += abs(smooth_snoise(v)) * scale;
		v *= vec2(2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}

float snoise_abs(in vec3 v, int octaves) {
	float res = 0.0;
	float scale = 1.0;
	for(int i=0; i<8; i++) {
		if(i >= octaves) break;
		res += abs(smooth_snoise(v)) * scale;
		v *= vec3(2.0, 2.0, 2.0);
		scale *= 0.5;
	}
	return res;
}

vec3 mod289(in vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(in vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(in vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float smooth_snoise(in vec2 v){
    const vec4 C = vec4(0.211324865405187,
                        0.366025403784439, 
                        -0.577350269189626,
                        0.024390243902439);
    // First corner
    vec2 i = floor(v + dot(v, C.yy) );
    vec2 x0 = v - i + dot(i, C.xx);

    // Other corners
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i);
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
    + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

vec4 mod289(in vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(in vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(in vec4 r){
    return 1.79284291400159 - 0.85373472095314 * r;
}

float smooth_snoise(in vec3 v){
    const vec2 C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i = floor(v + dot(v, C.yyy) );
    vec3 x0 = v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; 
    vec3 x3 = x0 - D.yyy;

    // Permutations
    i = mod289(i);
    vec4 p = permute( permute( permute(
                i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
            + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
            + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
    float n_ = 0.142857142857;
    vec3 ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                    dot(p2,x2), dot(p3,x3) ) );
}
// ************************************************ //


float simplexPerlin(vec2 pos){
    float alpha = 0.05f;
    float beta = 0.65f;
    float v1 = sin(pos.x);
    float v2 = cos(pos.y);
    // return beta*snoise_abs(vec2(v1,v2), 5);
    return beta*snoise(vec2(v1,v2), 5);
}

float windField(vec2 uv, float dt, vec2 flowDirection) {
    const float speed = 0.8f;  // Adjust the speed of the wind
    const int octaves = 5;    // Adjust the number of octaves
    const float persistence = .5f;  // Adjust the persistence of the noise
    float scale = 1.f;  // Adjust the scale of the noise

    vec2 movingUV = uv + flowDirection * dt * speed;
    // generate wind field using multiple octaves of Simplex noise
    float windFieldStrength = 0.f;
    float amplitude = 1.f;

    for (int i = 0; i < octaves; i++) {
        windFieldStrength += amplitude * snoise(movingUV * scale, i);
        scale /= 4.0f;  // Adjust the scale for each octave
        amplitude *= persistence;  // Adjust the amplitude for each octave
    }

    return windFieldStrength; // [-1, 1]
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

vec3 getAnimatedPos(vec3 basePos, float height, float noise){
    // sine
    // phase affected by per-blade hash + position along grass blade
    float theta = noise;
    float phase = basePos.y / height;
    float amplitude = .5f;

    float yDelta = basePos.y == 0.f ? 0.f : amplitude*sin(theta + phase);
    float xDelta = basePos.y == 0.f ? 0.f : amplitude*sin(theta + phase);

    return vec3(basePos.x + xDelta, basePos.y + yDelta, basePos.z);
}

vec2 getAnimatedPos(vec2 basePos, float height, float noise){
    float theta = noise;

    float phaseY = basePos.y / height;
    float amplitudeY = 0.05f;
    float frequencyY = 5.f * phaseY;

    float yDelta = amplitudeY*sin(frequencyY*theta + phaseY);
    // float yDelta = 0.f;
    float newY = basePos.y + yDelta;

    float phaseX = newY / height;
    float amplitudeX = 0.1f;
    float frequencyX = 2.f * phaseX;

    float xDelta = amplitudeX*sin(frequencyX*theta + phaseX);
    // float xDelta = 0.f;
    float newX = basePos.x + xDelta;

    return vec2(newX, newY);
}

void getVerticesPositionsAndNormals(vec3 pos, float width, float height, float tilt, vec2 bend, vec3 color,
    vec2 flowDirection,
    out vec3 positions[NB_VERT_HIGH_LOD],
    out vec3 normals[NB_VERT_HIGH_LOD],
    out vec3 colors[NB_VERT_HIGH_LOD]
    ){
    int nbVert = tileLOD == HIGH_LOD ? NB_VERT_HIGH_LOD : NB_VERT_LOW_LOD;

    float minWidth = width / 5.f;
    float widthDelta = minWidth / nbVert;
    float curWidth = width / 2.f;

    // float noise = infinitePerlin(pos.xz);
    float noise = windField(pos.xz, time, flowDirection);
    float noiseColor = windField(pos.xz, time, flowDirection);

    // vec2 deltaSubtle = vec2(0.1f*abs(noiseSubtle), 0.01f*noiseSubtle);

    vec2 P0 = vec2(0.f);
    vec2 P1 = bend;
    vec2 P2 = vec2(tilt, height);
    // vec2 P1 = bend + deltaSubtle;
    // vec2 P2 = vec2(tilt, height) + deltaSubtle;
    P1 = getAnimatedPos(P1, height, noise);
    P2 = getAnimatedPos(P2, height, noise);

    vec3 widthTangent = vec3(0.f, 0.f, 1.f);

    for(int i=0; i<nbVert-1; i+=2){
        float t = i / (1.f * nbVert);
        vec2 bendAndTilt = quadraticBezierCurve(t, P0, P1, P2);
        vec3 newPosLeft = pos + vec3(bendAndTilt.x, bendAndTilt.y, -curWidth);
        vec3 newPosRight = pos + vec3(bendAndTilt.x, bendAndTilt.y, curWidth);
        positions[i] = newPosLeft;
        positions[i+1] = newPosRight;
        // positions[i] = getAnimatedPos(newPosLeft, height, noise);
        // positions[i+1] = getAnimatedPos(newPosRight, height, noise);
        colors[i] = getColor(color, height, bendAndTilt.y);
        colors[i+1] = getColor(color, height, bendAndTilt.y);
        // colors[i] = noiseColor*vec3(1.f,1.f,1.f);
        // colors[i+1] = noiseColor*vec3(1.f,1.f,1.f);
        curWidth -= widthDelta;

        vec2 bezierDerivative = quadraticBezierCurveDerivative(t, P0, P1, P2);
        vec3 bezierNormal = normalize(vec3(bezierDerivative.x, bezierDerivative.y, 0.f));
        vec3 normal = cross(bezierNormal, widthTangent);
        // rotate the normals a bit
        // normals[i] = normal;
        // normals[i+1] = normal;
        normals[i] = normalize(getRotationMatrix(PI * (-0.3f)) * normal);
        normals[i+1] = normalize(getRotationMatrix(PI * 0.3f) * normal);
    }

    vec3 newPos = pos + vec3(P2, 0.f);
    positions[nbVert-1] = newPos;
    // positions[nbVert-1] = getAnimatedPos(newPos, height, noise);
    vec2 bezierDerivative = quadraticBezierCurveDerivative(1.f, P0, P1, P2);
    vec3 bezierNormal = normalize(vec3(bezierDerivative.x, bezierDerivative.y, 0.f));
    vec3 normal = cross(bezierNormal, widthTangent);
    normals[nbVert-1] = normal;
    colors[nbVert-1] = TIP_COLOR;
    // colors[nbVert-1] = noiseColor*vec3(1.f,1.f,1.f);
}

vec3 getModelPos(vec3 center, vec3 position, float rotation){
    return getRotationMatrix(rotation) * (position - center) + center;
}

vec3 getModelPos(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], int vertex, float rotation){
    return getModelPos(center, positions[vertex], rotation);
}

vec4 getWorldPos(vec3 center, vec3 position, float rotation, vec2 flowDirection, float height){
    vec3 curPosition = getModelPos(center, position, rotation);
    // test flow direction
    float noise = windField(center.xz, time, flowDirection); // [-1, 1]
    float factor = 0.5f * (position.y / height);
    vec3 direction = vec3(flowDirection.x, 0.f, flowDirection.y);
    curPosition += noise * factor * direction; 

    vec4 worldPosition = proj * view * vec4(curPosition, 1.f);
    return worldPosition;
}

vec4 getWorldPos(vec3 center, vec3 positions[NB_VERT_HIGH_LOD], int vertex, float rotation, vec2 flowDirection, float height){
    return getWorldPos(center, positions[vertex], rotation, flowDirection, height);
}

vec3 getRotatedNormals(vec3 normal, float rotation){
    return normalize(getRotationMatrix(rotation) * normal);
}

void createTriangle(vec3 center, 
    vec3 positions[NB_VERT_HIGH_LOD], 
    vec3 normals[NB_VERT_HIGH_LOD], 
    vec3 colors[NB_VERT_HIGH_LOD],
    float height, float rotation, vec3 color, int indices[3], vec2 flowDirection){
    int idx = 0;
    for(int i=0; i<3; i++){
        idx = indices[i];
        vec3 modelPos = getModelPos(center, positions, idx, rotation);
        vec4 worldPos = getWorldPos(center, positions, idx, rotation, flowDirection, height);
        vec3 normal = getRotatedNormals(normals[idx], rotation);
        geomFragCol = colors[idx];
        geomFragNormal = normal;
        geomFragPos = modelPos;
        gl_Position = worldPos;
        EmitVertex();    
    }
    EndPrimitive();
}

void createTriangles(vec3 center, float rotation, vec3 color, float height,
    vec2 flowDirection,
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
        createTriangle(center, positions, normals, colors, height, rotation, color, indices, flowDirection);

        // second triangle
        indices[0] = vertCounter+2;
        indices[1] = vertCounter+1;
        indices[2] = vertCounter+3;
        createTriangle(center, positions, normals, colors, height, rotation, color, indices, flowDirection);

        vertCounter += 2;
    }
    // last triangle
    indices[0] = nbVert-3;
    indices[1] = nbVert-2;
    indices[2] = nbVert-1;
    createTriangle(center, positions, normals, colors, height, rotation, color, indices, flowDirection);
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
    vec2 flowDirection = normalize(vec2(1.0, 0.5));  // Adjust the main wind direction

    getVerticesPositionsAndNormals(pos, width, height, tilt, bend, color, flowDirection, positions, normals, colors);
    createTriangles(pos, rotation, color, height, flowDirection, positions, normals, colors);

}