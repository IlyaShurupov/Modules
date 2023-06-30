#pragma once

#include "Vec.hpp"

namespace tp {

	class RGB;
	class HSV;

	class RGB {
	public:
		RGB();
		RGB(flt4 pr, flt4 pg, flt4 pb);
		RGB(flt4 val);

	public:
		void set(flt4 pr, flt4 pg, flt4 pb);
		operator HSV() const;

	public:
		halnf r, g, b;
	};

	class HSV {
	public:
		HSV();
		HSV(flt4 ph, flt4 ps, flt4 pv);

	public:
		void set(flt4 ph, flt4 ps, flt4 pv);
		operator RGB() const;

	public:
		halnf h, s, v;
	};

	class RGBA {
	public:
		RGBA() : r(0), g(0), b(0), a(0) {}
		RGBA(flt4 val) : rgbs(val), a(val) {}
		RGBA(const RGB& RGBs, flt4 val) : rgbs(RGBs), a(val) {}
		RGBA(flt4 pr, flt4 pg, flt4 pb, flt4 pa) : rgbs(pr, pg, pb), a(pa) {}

	public:
		RGBA& operator=(const HSV& in) {
			rgbs = in;
			a = 1;
			return *this;
		}

		RGBA operator-(const RGBA& in) const {
			auto const nr = tp::clamp(r - in.r, 0.f, 1.f);
			auto const ng = tp::clamp(g - in.g, 0.f, 1.f);
			auto const nb = tp::clamp(b - in.b, 0.f, 1.f);
			auto const na = tp::clamp(a - in.a, 0.f, 1.f);
			return { nr, ng, nb, na };
		}

		RGBA operator+(const RGBA& in) const {
			auto const nr = tp::clamp(r + in.r, 0.f, 1.f);
			auto const ng = tp::clamp(g + in.g, 0.f, 1.f);
			auto const nb = tp::clamp(b + in.b, 0.f, 1.f);
			auto const na = tp::clamp(a + in.a, 0.f, 1.f);
			return { nr, ng, nb, na };
		}

	public:
		flt4 a;

		union {
			RGB rgbs;
			struct {
				flt4 r;
				flt4 g;
				flt4 b;
			};
		};
	};

	class HSVA {
		HSV rgbs;
		flt4 a = 1.f;
	};
}
