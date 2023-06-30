
#include "Ray.hpp"

using namespace tp;

Ray::Ray(const Vec3F& aDir, const Vec3F& aPos) {
	this->dir = aDir.unitV();
	this->pos = aPos;
}