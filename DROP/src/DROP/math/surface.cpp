#pragma once
#include "../math/surface.h"

using namespace VgMath;

Versor3 Surface::Normal() const{
	Versor3 up = Versor3::up();
	up = (*m_Transform)(up);
	return up;
}

void Surface::Normal(Versor3& outNormal) const {
	outNormal.x = 0.0f;
	outNormal.y = 1.0f;
	outNormal.z = 0.0f;

	outNormal = (*m_Transform)(outNormal);
}

Point3 Surface::RandomPointOnSurface() const {
	Point3 pointOnSurface(
		randomBetween(-m_Size.x, m_Size.x)
		, 0.0f
		, randomBetween(-m_Size.y, m_Size.y)
	);

	return pointOnSurface = (*m_Transform)(pointOnSurface);
}

void Surface::RandomPointOnSurface(Point3& outPoint) const {
	outPoint.x = randomBetween(-m_Size.x, m_Size.x);
	outPoint.y = randomBetween(-m_Size.y, m_Size.y);
	outPoint.z = 0.0f;

	outPoint = (*m_Transform)(outPoint);
}
