#pragma once

#include "Vec.hpp"

namespace tp {

	class Ray {
	public:
		Ray(const Vec3F& Dir, const  Vec3F& Pos);
		Ray() = default;

	public:
		Vec3F dir;
		Vec3F pos;

	public:
		~Ray() = default;
	};

}