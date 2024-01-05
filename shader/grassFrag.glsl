#version 450 core

out vec4 oFragCol;

in vec3 geomFragCol;
in vec3 geomFragNormal;
in vec3 geomFragPos;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 camPos;
uniform float fAmbient;
uniform float fDiffuse;
uniform float fSpecular;
uniform float fShininess;

struct PointLight{
    vec3 position;
    vec3 color;
};

const int MAX_SIZE = 128;
uniform int nbPointLights;
uniform PointLight pointLights[MAX_SIZE];

vec3 getLightPos(PointLight light){
    // return (proj*view*vec4(light.position, 1.f)).xyz;
    return light.position;
}

vec3 getAmbient(vec3 lColor, vec3 oColor){
    return fAmbient * lColor * oColor;
}

vec3 getNormal(){
    vec3 norm    = normalize(geomFragNormal);
    vec3 viewDir = normalize(camPos - geomFragPos);

    if (!gl_FrontFacing)
        norm = -norm;

    return norm;
}

vec3 getDiffuse(vec3 lPos, vec3 lColor, vec3 oColor){
    vec3 lDir = normalize(lPos-geomFragPos);
    vec3 nDir = getNormal();
    vec3 c = vec3(oColor.x*lColor.x, oColor.y*lColor.y, oColor.z*lColor.z);
    return fDiffuse*max(0.f, dot(nDir, lDir))*c;
}

vec3 getSpecular(vec3 lPos, vec3 lColor, vec3 oColor){
    vec3 lDir = normalize(lPos-geomFragPos);
    vec3 camDir = normalize(camPos-geomFragPos);
    vec3 nDir = getNormal();

    vec3 h = normalize(lDir + camDir);
    vec3 c = vec3(oColor.x*lColor.x, oColor.y*lColor.y, oColor.z*lColor.z);

    return fSpecular*pow(max(0.f, dot(nDir, h)), fShininess)*c;
}


vec3 getAmbient(vec3 oColor){
    vec3 sum = vec3(0.f);
    for(int i=0; i<nbPointLights; i++){
        vec3 lColor = pointLights[i].color;
        sum += getAmbient(lColor, oColor);
    }
    return sum;
}


vec3 getDiffuse(vec3 oColor){
    vec3 sum = vec3(0.f);
    for(int i=0; i<nbPointLights; i++){
        vec3 lPos = getLightPos(pointLights[i]);
        vec3 lColor = pointLights[i].color;
        sum += getDiffuse(lPos, lColor, oColor);
    }
    return sum;
}


vec3 getSpecular(vec3 oColor){
    vec3 sum = vec3(0.f);
    for(int i=0; i<nbPointLights; i++){
        vec3 lPos = getLightPos(pointLights[i]);
        vec3 lColor = pointLights[i].color;
        sum += getSpecular(lPos, lColor, oColor);
    }
    return sum;
}
	

void main(){
    vec3 color = geomFragCol;
    // vec3 color = geomFragNormal;
    vec3 ambient = getAmbient(geomFragCol);
    vec3 diffuse = getDiffuse(geomFragCol);
    vec3 specular = getSpecular(geomFragCol);
    oFragCol = vec4(ambient + diffuse + specular, 1.f);
    // oFragCol = vec4(geomFragNormal, 1.f);
}