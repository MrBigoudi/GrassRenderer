#pragma once

#include <glm/glm.hpp>

bool doCircleRectangleIntersect(
            const glm::vec3& circleCenter,
            float circleRadius,
            const glm::vec3& rectangleUpLeft,
            const glm::vec3& rectangleUpRight,
            const glm::vec3& rectangleDownLeft,
            const glm::vec3& rectangleDownRight);