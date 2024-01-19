#include "utils.hpp"
#include <iostream>
#include <glm/geometric.hpp>

bool doCircleRectangleIntersect(
            const glm::vec3& circleCenter,
            float circleRadius,
            const glm::vec3& rectangleUpLeft,
            const glm::vec3& rectangleUpRight,
            const glm::vec3& rectangleDownLeft,
            const glm::vec3& rectangleDownRight){

    float rectWidth = rectangleUpRight.x - rectangleUpLeft.x;
    float rectHeight = rectangleUpLeft.z - rectangleDownLeft.z;
    float rectHalfWidth = rectWidth / 2.f;
    float rectHalfHeight = rectHeight / 2.f;
    glm::vec3 rectCenter = rectangleUpLeft + glm::vec3(rectHalfWidth, 0.f, rectHalfHeight);

    float distRectCicrle = glm::distance(circleCenter, rectCenter);
    if(distRectCicrle > circleRadius) return false;
    if(distRectCicrle > circleRadius) return false;

    return true;
}

void displayTime(std::chrono::high_resolution_clock::time_point start,
                std::chrono::high_resolution_clock::time_point end,
                const std::string& msg){
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if(msg != "")
        fprintf(stdout, "Time elapsed in %s: %ld microseconds\n", msg.c_str(), duration.count());
    else
        fprintf(stdout, "Time elapsed: %ld microseconds\n", duration.count());
}

void printGlm(const glm::vec2& vec){
    std::cout << "(" 
        << vec.x 
        << ", " << vec.y
        << ")";
}

void printGlm(const glm::vec3& vec){
    std::cout << "(" 
        << vec.x 
        << ", " << vec.y
        << ", " << vec.z 
        << ")";
}

void printGlm(const glm::vec4& vec){
    std::cout << "(" 
        << vec.x 
        << ", " << vec.y
        << ", " << vec.z 
        << ", " << vec.w 
        << ")";
}