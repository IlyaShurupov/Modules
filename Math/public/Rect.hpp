
#pragma once

#include "Vec.hpp"

#include "Intersections.hpp"

namespace tp {

	template <typename Type> class Rect;
	using RectF = Rect<halnf>;
	using RectI = Rect<halni>;

	template <typename Type>
	class Rect {
	public:
		Rect() {}

		explicit Rect(Type val) {
			this->pos = val;
			this->size = val;
		}

		template <typename ConversionType>
		explicit Rect(const Rect<ConversionType>& rec) {
			this->pos = rec.pos;
			this->size = rec.size;
		}

		Rect(const Vec2<Type>& pos, const Vec2<Type>& size) {
			this->pos = pos;
			this->size = size;
		}

		Rect(Type aPosX, Type posy, Type aSizeX, Type aSizeY) {
			pos.assign(aPosX, posy);
			size.assign(aSizeX, aSizeY);
		}

		// assign
		template <typename InType>
		Rect<Type>& assign(InType p1x, InType p1y, InType p2x, InType p2y) {
			pos.assign(p1x, p1y);
			size.assign(p2x, p2y);
			return *this;
		}

		// assign
		template <typename InType>
		Rect<Type>& assign(const Vec2<InType>& pos, const Vec2<InType>& size) {
			this->pos.assign(pos.x, pos.y);
			this->size.assign(size.x, size.y);
			return *this;
		}

		// conversion
		template <typename ConversionType>
		Rect<Type>& operator=(const Rect<ConversionType>& rect) {
			pos = rect.pos;
			size = rect.size;
			return *this;
		}

		bool operator==(Rect<Type>& rect) const {
			return (pos == rect.pos && size == rect.size);
		}

		bool isEnclosedIn(const Rect<Type>& rect, bool aParent = false) const {
			if (aParent) {
				return (pos.x + size.x <= rect.size.x && pos.y + size.y <= rect.size.y &&
					pos.x >= 0 && pos.y >= 0);
			}
			*(Vec2<Type>*)(&pos) -= rect.pos;
			bool ret = this->isEnclosedIn(rect, true);
			*(Vec2<Type>*)(&pos) += rect.pos;
			return ret;
		}

		void calcIntersection(Rect<Type>& in, Rect<Type>& out) const {
			if (isOverlap(in)) {
				out = *this;
				for (char i = 0; i < 2; i++) {
					clamp(out.pos[i], in.pos[i], in.pos[i] + in.size[i]);
					Type p2 = pos[i] + size[i];
					clamp(p2, in.pos[i], in.pos[i] + in.size[i]);
					out.size[i] = p2 - out.pos[i];
				}
			}
			else {
				out.size.assign(0, 0);
				out.pos.assign(0, 0);
			}
		}

		// argument isInside
		bool isInside(const Vec2<Type>& p) const {
			return (p.each_compre(pos) && (pos + size).each_compre(p));
		}

		bool isInside(Type x, Type y) const {
			return (pos.x < x&& pos.y < y&& pos.x + size.x > x&& pos.y + size.y > y);
		}

		inline Vec2<Type> sizeVec() const {
			return Vec2<Type>(size.x, size.y);
		}

		inline Vec2<Type> sizeVecW() const {
			return Vec2<Type>(size.x + pos.x, size.y + pos.y);
		}

		void invertY(Type scr_y) {
			pos.y = scr_y - pos.y - size.y;
		}

		void move(Type dx, Type dy) {
			pos.x += dx;
			pos.y += dy;
		}

		Rect<Type>& scaleFromCenter(tp::halnf fac, bool add = false) {
			if (add) {
				pos += fac;
				size -= fac * 2;
			}
			else {
				auto new_size = size * fac;
				pos = pos - (new_size - size) / 2;
				size = new_size;
			}
			return *this;
		}

		Vec2<Type> p1() {
			return pos;
		}

		Vec2<Type> p3() {
			return pos + size;
		}

		Vec2<Type> p2() {
			return { pos.x, pos.y + size.y };
		}

		Vec2<Type> p4() {
			return { pos.x + size.x, pos.y };
		}

		inline bool isAbove(const Rect<Type>& rect) const {
			return (pos.y + size.y < rect.pos.y);
		}

		inline bool isBellow(const Rect<Type>& rect) const {
			return (rect.pos.y + rect.size.y < pos.y);
		}

		inline bool isRight(const Rect<Type>& rect) const {
			return (pos.x + size.x < rect.pos.x);
		}

		inline bool isLeft(const Rect<Type>& rect) const {
			return (rect.pos.x + rect.size.x < pos.x);
		}

		inline bool isIntersectsY(const Rect<Type>& in) const {
			if (INRANGE(in.pos.x, pos.x, pos.x + size.x)) return true;
			if (INRANGE(pos.x, in.pos.x, in.pos.x + in.size.x)) return true;
			return false;
		}

		inline bool isIntersectX(const Rect<Type>& rect) const {
			if (INRANGE(rect.pos.y, pos.y, pos.y + size.y)) return true;
			if (INRANGE(pos.y, rect.pos.y, rect.pos.y + rect.size.y)) return true;
			return false;
		}

		bool isOverlap(const Rect<Type>& rect) const {
			return (isIntersectX(rect) && isIntersectsY(rect));
		}

		void clamp(const Rect<Type>& bounds) {
			Vec2<Type> p3(pos + size);
			Vec2<Type> max = bounds.pos + bounds.size;

			pos.clamp(bounds.pos, max);
			p3.clamp(bounds.pos, max);

			size = p3 - pos;
		}

		// if only one point isInside
		bool clampOutside(Vec2<Type>& v1, Vec2<Type>& v2) {
			bool const in1 = isInside(v1);
			bool const in2 = isInside(v2);
			if (!in1 && !in2) return false;

			if (in1) {
				if (!intersectLines2D(p2(), p3(), v1, v2, &v1)) {
					if (!intersectLines2D(p1(), p4(), v1, v2, &v1)) {
						if (!intersectLines2D(p1(), p2(), v1, v2, &v1)) {
							intersectLines2D(p3(), p4(), v1, v2, &v1);
						}
					}
				}
				return true;
			}

			if (!intersectLines2D(p2(), p3(), v1, v2, &v2)) {
				if (!intersectLines2D(p1(), p4(), v1, v2, &v2)) {
					if (!intersectLines2D(p1(), p2(), v1, v2, &v2)) {
						intersectLines2D(p3(), p4(), v1, v2, &v2);
					}
				}
			}
			return true;
		}

		uhalni clampInside(Vec2<Type>& v1, Vec2<Type>& v2) {
			bool const in1 = isInside(v1);
			bool const in2 = isInside(v2);

			if (in1 && in2) return 2;

			Vec2<Type> v1copy = v1;
			Vec2<Type> v2copy = v2;

			if (in1 || in2) {
				if (!intersectLines2D(p2(), p3(), v1copy, v2copy, &v2)) {
					if (!intersectLines2D(p1(), p4(), v1copy, v2copy, &v2)) {
						if (!intersectLines2D(p1(), p2(), v1copy, v2copy, &v2)) {
							intersectLines2D(p3(), p4(), v1copy, v2copy, &v2);
						}
					}
				}
				return 1;
			}

			DEBUG_ASSERT(0)
			return 0;
		}

		template <typename ConversionType>
		Rect<Type> operator*(ConversionType val) {
			Rect<Type> out;
			out.pos = pos * val;
			out.size = size * val;
			return out;
		}

		Vec2<Type> center() {
			return pos + size / 2.f;
		}
	
	public:
		union {
			Vec2<Type> v1;
			Vec2<Type> pos;
			struct {
				Type x;
				Type y;
			};
		};

		union {
			Vec2<Type> v2;
			Vec2<Type> size;
			struct {
				Type z;
				Type w;
			};
		};
	};
}