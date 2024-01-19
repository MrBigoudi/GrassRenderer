#pragma once

#include <glm/glm.hpp>

struct Plane{
    glm::vec3 _Normal = {0.f, 1.f, 0.f};
    // distance from origin to the nearest point in the plane
    float _Distance = 0.f;

    Plane() = default;

	Plane(const glm::vec3& p1, const glm::vec3& norm)
		: _Normal(glm::normalize(norm)),
		_Distance(glm::dot(_Normal, p1))
	{}

	float getSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(_Normal, point) - _Distance;
	}
};

struct Frustum{
    Plane _TopFace;
    Plane _BottomFace;
    Plane _LeftFace;
    Plane _RightFace;
    Plane _FarFace;
    Plane _NearFace;
};