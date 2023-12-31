#version 450 core

// Buffers and layouts

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

layout(binding = 4, std430) buffer ColorsBuffer {
    vec3 colors[];
};

layout(binding = 5, std430) buffer RotationsBuffer {
    float rotations[];
};



// Uniform variables

uniform int tileWidth;
uniform int tileHeight;
uniform int gridNbCols;
uniform int gridNbLines;
uniform vec2 tilePos;
uniform int tileID;
const float PI = 3.1416f;


// Helper functions

// generate random value given a vec2 seed
float rand(vec2 co){
    return fract(sin(dot(co*(tileID+1) ,vec2(12.9898,78.233))) * 43758.5453);
}

// return the id of the grid cell given a position
uint getGridCell(vec2 position){
    float cellWidth = tileWidth / gridNbCols;
    float cellHeight = tileHeight / gridNbLines;

    uint cellX = uint(position.x / cellWidth);
    uint cellY = uint(position.y / cellHeight);

    return cellX + cellY * gridNbCols;
}

bool cellIsFirstRow(uint cellId){
    return cellId < gridNbCols;
}

bool cellIsLastRow(uint cellId){
    return cellId > gridNbCols*(gridNbLines-1);
}

bool cellIsFirstCol(uint cellId){
    return (cellId % gridNbLines) == 0;
}

bool cellIsLastCol(uint cellId){
    return (cellId % gridNbLines) == (gridNbCols-1);
}

// return the ids of the closest intersections to the blade position and return the number of neighbors to check
uint getClosestIntersections(vec3 bladePosition, out uint resultIds[9]){
    vec2 positionTmp = bladePosition.xz;
    uint cellId = getGridCell(positionTmp);

    // center cell
    resultIds[0] = cellId;
    uint curIdx = 1;
    // up cell
    if(!cellIsFirstRow(cellId)){
        resultIds[curIdx] = cellId - gridNbCols; 
        curIdx++; 
    }
    // down cell
    if(!cellIsLastRow(cellId)){
        resultIds[curIdx] = cellId + gridNbCols; 
        curIdx++; 
    }
    // left cell
    if(!cellIsFirstCol(cellId)){
        resultIds[curIdx] = cellId - 1; 
        curIdx++;
    }
    // right cell
    if(!cellIsLastCol(cellId)){
        resultIds[curIdx] = cellId + 1; 
        curIdx++;
    }
    // up left
    if(!cellIsFirstRow(cellId) && !cellIsFirstCol(cellId)){
        resultIds[curIdx] = cellId - gridNbCols - 1;
        curIdx++;
    }
    // up right
    if(!cellIsFirstRow(cellId) && !cellIsLastCol(cellId)){
        resultIds[curIdx] = cellId - gridNbCols + 1;
        curIdx++;
    }
    // down left
    if(!cellIsLastRow(cellId) && !cellIsFirstCol(cellId)){
        resultIds[curIdx] = cellId + gridNbCols - 1;
        curIdx++;
    }
    // down right
    if(!cellIsLastRow(cellId) && !cellIsLastCol(cellId)){
        resultIds[curIdx] = cellId + gridNbCols + 1;
        curIdx++;
    }

    return curIdx;
}

vec2 getIntersectionPosition(uint cellId){
    float randX = rand(vec2(cellId, 0.f));
    float randZ = rand(vec2(0.f, cellId));

    float cellX = cellId % gridNbCols;
    float cellZ = cellId / gridNbLines;

    return vec2(cellX, cellZ);
}

// return the jittered positions of the closest intersections to the blade position
void getVoronoiPositions(uint nbIntersections, uint voronoiCellIds[9], out vec2 voronoiPositions[9]){
    for(uint i=0; i<nbIntersections; i++){
        voronoiPositions[i] = getIntersectionPosition(voronoiCellIds[i]);
    }
}

// return the id of the nearest neighbour
uint findNearestNeighbour(vec3 bladePosition, uint nbIntersections, vec2 voronoiPositions[9]){
    vec2 positionTmp = bladePosition.xz;
    float minDist = distance(positionTmp, voronoiPositions[0]);
    uint bestId = 0;

    for(uint i=1; i<nbIntersections; i++){
        float curDist = distance(positionTmp, voronoiPositions[i]);
        if(curDist < minDist){
            minDist = curDist;
            bestId = i;
        }
    }

    return bestId;
}



// Main functions

// give a random position in the tile for the blade
vec3 getRandomPosition(uint id){
    vec3 newPos = vec3(0.f);

    float randX = rand(vec2(id, 0.f));
    float randZ = rand(vec2(0.f, id));

    newPos.x = randX * float(tileWidth);
    newPos.z = randZ * float(tileHeight);

    vec3 curTilePos = vec3(tilePos.x, 0.f, tilePos.y);

    return newPos + curTilePos;
}

// find the clump in which the grass blade is
uint getClumpId(vec3 bladePosition){
    // find the nearest intersections ids
    uint intersections[9];
    uint nbIntersections = getClosestIntersections(bladePosition, intersections);
    // get their jittered positions
    vec2 voronoiPositions[9];
    getVoronoiPositions(nbIntersections, intersections, voronoiPositions);
    // find nearest of them
    uint nearestId = findNearestNeighbour(bladePosition, nbIntersections, voronoiPositions);
    // return the clump id
    return intersections[nearestId];
}

void main() {
    uint instanceIndex = gl_GlobalInvocationID.x;
    // Store data in buffers

    vec3 bladePosition = getRandomPosition(instanceIndex);
    positions[instanceIndex] = bladePosition;
    uint clumpId = getClumpId(bladePosition);

    facings[instanceIndex] = vec2(0.f);
    heights[instanceIndex] = 0.5f;
    widths[instanceIndex] = 0.1f;

    vec3 color = vec3(
        0.f,
        rand(vec2(clumpId, clumpId)),
        0.f
    );
    colors[instanceIndex] = color;

    float rotation = radians(rand(vec2(instanceIndex, 0.f)) * 360.f);
    rotations[instanceIndex] = rotation;
}