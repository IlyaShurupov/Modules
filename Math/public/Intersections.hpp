#pragma once

#include "Vec.hpp"

namespace tp {

	template <typename Type>
	bool intersectLines2D(const Vec2<Type>& p1, const Vec2<Type>& p2, const Vec2<Type>& v1, const Vec2<Type>& v2, Vec2<Type>* out) {
		auto a1 = p2.x - p1.x;
		auto a2 = p2.y - p1.y;

		auto b1 = v2.x - v1.x;
		auto b2 = v2.y - v1.y;

		auto c1 = v1.x - p1.x;
		auto c2 = v1.y - p1.y;

		auto det = a2 * b1 - a1 * b2;

		auto t1 = ( -b2 * c1 + b1 * c2 ) / det;
		auto t2 = ( -a2 * c1 + a1 * c2 ) / det;

		if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
			if (out != nullptr) {
				out->x = p1.x + (a1 * t1);
				out->y = p1.y + (a2 * t1);
			}
			return true;
		}
		return false;
	}

}

