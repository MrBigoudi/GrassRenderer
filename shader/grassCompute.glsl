#version 450 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, std430) buffer PositionsBuffer {
    vec3 positions[];
};

layout(binding = 1, std430) buffer FacingsBuffer {
    vec2 facings[];
};

layout(binding = 2, std430) buffer HeightsBuffer {
    float heights[];
};

layout(binding = 3, std430) buffer WidthsBuffer {
    float widths[];
};

uniform int tileWidth;
uniform int tileHeight;
uniform int tileLength;


vec3 getPosition(uint id){
    vec3 newPos = vec3(0.f);
    uint tileX = id;
    // uint tileX = id % tileWidth;
    uint tileY = 0;
    // uint tileY = id / tileHeight;

    newPos.x = tileX * tileLength; // center to the tile for now
    newPos.y = tileY * tileLength; // center to the tile for now
    newPos.z = 0.f;

    return newPos;
}


void main() {
    uint instanceIndex = gl_GlobalInvocationID.x;
    // Store data in buffers
    positions[instanceIndex] = getPosition(instanceIndex);
    facings[instanceIndex] = vec2(0.f);
    heights[instanceIndex] = 0.5f;
    widths[instanceIndex] = 0.1f;
}