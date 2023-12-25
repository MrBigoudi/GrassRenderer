#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vCol;
layout(location = 2) in vec2 vUvs;
layout(location = 3) in vec3 vNorm;

out vec4 fCol;
out vec3 fNorm;
out vec3 fPos;
out vec2 fUvs;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

vec4 getPositions(){
    mat4 MVP = projMat * viewMat * modelMat;
    return MVP * vec4(vPos, 1.0);
}

vec3 getNormals(){
    mat3 normalMatrix = transpose(inverse(mat3(modelMat)));
    return normalize(normalMatrix * vNorm);
}

void main(){
    gl_Position = getPositions();
    fCol = vCol;
    fNorm = getNormals();
    fPos = vec3(modelMat*vec4(vPos, 1.0));
    fUvs = vUvs;
}