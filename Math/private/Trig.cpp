
#include "Trig.hpp"

#include "Ray.hpp"

using namespace tp;

Trig::Trig() {
	MODULE_SANITY_CHECK(gModuleMath)
	p1.assign(0.f, 0.f, 0.f);
	p2.assign(0.f, 0.f, 0.f);
	p3.assign(0.f, 0.f, 0.f);
}

Trig::Trig(const Vec3F& v0, const Vec3F& v1, const Vec3F& v2) {
	p1 = v0;
	p2 = v1;
	p3 = v2;
}

void Trig::assign(const Vec3F& v0, const Vec3F& v1, const Vec3F& v2) {
	p1 = v0;
	p2 = v1;
	p3 = v2;
}

Trig::~Trig() = default;

void Trig::normal(Vec3F& dir) const {
	dir = (p2 - p1).cross(p3 - p1);
	dir.normalize();
}

Vec3F edge1, edge2, h, s, q;
halnf a, f, u, v;
halnf t;

bool Trig::rayHit(class Ray& ray, Vec3F& HitPos) const {

	edge1 = p2 - p1;
	edge2 = p3 - p1;

	h = ray.dir.cross(edge2);
	a = edge1.dot(h);

	if (a > -EPSILON && a < EPSILON) {
		return false;
	}

	f = 1.f / a;
	s = ray.pos - p1;
	u = f * s.dot(h);

	if (u < 0.0 || u > 1.0) {
		return false;
	}

	q = s.cross(edge1);
	v = f * ray.dir.dot(q);

	if (v < 0.f || u + v > 1.f) {
		return false;
	}

	t = f * edge2.dot(q);
	if (t > EPSILON) {
		HitPos = ray.pos + ray.dir * t;
		return true;
	} else {
		return false;
	}
}
