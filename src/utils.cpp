#include "utils.hpp"
#include <glm/geometric.hpp>

bool doCircleRectangleIntersect(
            const glm::vec3& circleCenter,
            float circleRadius,
            const glm::vec3& rectangleUpLeft,
            const glm::vec3& rectangleUpRight,
            const glm::vec3& rectangleDownLeft,
            const glm::vec3& rectangleDownRight){

    float rectWidth = rectangleUpLeft.x - rectangleUpRight.x;
    float rectHeight = rectangleUpLeft.z - rectangleDownLeft.z;
    float rectHalfWidth = rectWidth / 2.f;
    float rectHalfHeight = rectHeight / 2.f;
    glm::vec3 rectCenter = rectangleUpLeft + glm::vec3(rectHalfWidth, 0.f, rectHalfHeight);

    float distRectCicrle = glm::distance(circleCenter, rectCenter);
    if(distRectCicrle > (rectHalfWidth + circleRadius)) return false;
    if(distRectCicrle > (rectHalfHeight + circleRadius)) return false;

    return true;
}