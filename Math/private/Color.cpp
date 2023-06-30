
#include "Color.hpp"

using namespace tp;

RGB::RGB() { r = g = b = 1.f; }
RGB::RGB(flt4 pr, flt4 pg, flt4 pb) { set(pr, pg, pb); }
RGB::RGB(flt4 val) { set(val, val, val); }

void RGB::set(flt4 pr, flt4 pg, flt4 pb) {
	r = pr;
	b = pb;
	g = pg;
}

RGB::operator HSV() const {

	HSV out;
	alnf min, max, delta;

	min = r < g ? r : g;
	min = min < b ? min : b;
	max = r > g ? r : g;
	max = max > b ? max : b;

	out.v = (halnf) max;
	delta = max - min;

	if (delta < 0.00001) {
		out.s = 0;
		// undefined, maybe nan?
		out.h = 0;
		return out;
	}
	if (max > 0.f) {
		// NOTE: if Max is == 0, this divide would cause a crash
		out.s = (halnf) (delta / max);

	} else {
		// if max is 0, then r = g = b = 0
		// s = 0, h is undefined
		out.s = 0.f;
		out.h = 0.f;
		return out;
	}
	if (r >= max) {
		// between yellow & magenta
		out.h = (halnf) ((g - b) / delta);
	} else {
		if (g >= max) {
			// between cyan & yellow
			out.h = (halnf) (2.0 + (b - r) / delta);
		} else {
			// between magenta & cyan
			out.h = (halnf) (4.0 + (r - g) / delta);
		}
	}

	out.h *= 60.f;

	if (out.h < 0.0) {
		out.h += 360.f;
	}

	out.h = (halnf) (out.h / 360.f * (PI2));

	return out;
}

HSV::HSV() { h = s = v = 0.f; }

HSV::HSV(flt4 ph, flt4 ps, flt4 pv) { set(ph, ps, pv); }

void HSV::set(flt4 ph, flt4 ps, flt4 pv) {
	h = ph;
	s = ps;
	v = pv;
}

HSV::operator RGB() const {
	alnf hh, p, q, t, ff;
	alni i;
	RGB out;

	if (s <= 0.0) { // < is bogus, just shuts up warnings
		out.r = v;
		out.g = v;
		out.b = v;
		return out;
	}

	hh = h / (PI2) * 360;

	if (hh >= 360.0) {
		hh = 0.0;
	}

	hh /= 60.0;
	i = (long) hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
		case 0:
			out.r = (halnf) v;
			out.g = (halnf) t;
			out.b = (halnf) p;
			break;
		case 1:
			out.r = (halnf) q;
			out.g = (halnf) v;
			out.b = (halnf) p;
			break;
		case 2:
			out.r = (halnf) p;
			out.g = (halnf) v;
			out.b = (halnf) t;
			break;

		case 3:
			out.r = (halnf) p;
			out.g = (halnf) q;
			out.b = (halnf) v;
			break;
		case 4:
			out.r = (halnf) t;
			out.g = (halnf) p;
			out.b = (halnf) v;
			break;
		case 5:
		default:
			out.r = (halnf) v;
			out.g = (halnf) p;
			out.b = (halnf) q;
			break;
	}
	return out;
}

/*
struct rgbab {

	uint4 RGBA;

	rgbab() { RGBA = 0xFFFFFFFF; }
	rgbab(uint4 color) { RGBA = color; }
	rgbab(uint1 r, uint1 b, uint1 g, uint1 a) { set(r, b, g, a); }
	rgbab(const rgbab& in) { RGBA = in.RGBA; }

	void set(uint4 color) { RGBA = color; }
	void set(uint1 r, uint1 b, uint1 g, uint1 a) {
		RGBA = (uint1)(a);
		RGBA <<= 8; RGBA |= (uint1)(r);
		RGBA <<= 8; RGBA |= (uint1)(g);
		RGBA <<= 8; RGBA |= (uint1)(b);
	}

	operator rgbaf() {
		rgbaf out;
		out.r = uint1((RGBA & 0x00FF0000) >> 16) / 255.f;
		out.g = uint1((RGBA & 0x0000FF00) >> 8) / 255.f;
		out.b = uint1(RGBA & 0x000000FF) / 255.f;
		out.a = uint1((RGBA & 0xFF000000) >> 24) / 255.f;
		return out;
	}
};
*/