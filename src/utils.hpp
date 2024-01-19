#pragma once

#include <bits/chrono.h>
#include <string>
#include <glm/glm.hpp>

bool doCircleRectangleIntersect(
            const glm::vec3& circleCenter,
            float circleRadius,
            const glm::vec3& rectangleUpLeft,
            const glm::vec3& rectangleUpRight,
            const glm::vec3& rectangleDownLeft,
            const glm::vec3& rectangleDownRight);

void displayTime(std::chrono::high_resolution_clock::time_point start,
                std::chrono::high_resolution_clock::time_point end,
                const std::string& msg = "");

void printGlm(const glm::vec2& vec);
void printGlm(const glm::vec3& vec);
void printGlm(const glm::vec4& vec);