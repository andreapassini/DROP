#pragma once
#include "DROP/math/transform.h"

namespace VgMath {

// Represent a surface
// - Normal, the up vector of the transform
// - Size, the dimension of the surface
class Surface {
public:
	const Transform* m_Transform = nullptr;
	Vector2 m_Size{ 1.0f, 1.0f };
public:
	Versor3 Normal() const;
	void Normal(Versor3& outNormal) const;

	Point3 RandomPointOnSurface() const;
	void RandomPointOnSurface(Point3& outPoint) const;
};

}
