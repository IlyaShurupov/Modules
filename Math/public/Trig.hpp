#pragma once

#include "Vec.hpp"

namespace tp {

	class Trig {
	public:
		Trig();
		Trig(const Vec3F& v0, const Vec3F& v1, const Vec3F& v2);

	public:
		Vec3F p1;
		Vec3F p2;
		Vec3F p3;

	public:
		void assign(const Vec3F& v0, const Vec3F& v1, const Vec3F& v2);
		void normal(Vec3F& dir) const;
		bool rayHit(class Ray& ray, Vec3F& HitPos) const;

	public:
		~Trig();
	};
}