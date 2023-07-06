
#include "canvas.h"

#include "GraphicsLibApi.h"

#ifdef ENV_OS_ANDROID
const char* get_android_abs_path(const char* path, bool second = false);
#define NANOVG_GLES3_IMPLEMENTATION // Use GLES3 implementation.
#else
#define NANOVG_GL3_IMPLEMENTATION	// Use GL3 implementation.
#endif 

#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

using namespace tp;
using namespace glw;

#define NVG ((NVGcontext*) mCtx)

static const char* getFontPath() {
#ifdef ENV_OS_ANDROID
	return get_android_abs_path("rsc/fonts/CONSOLAB.TTF");
#else
	return "./rsc/fonts/CONSOLAB.TTF";
#endif

}

Canvas::Canvas() {

	// create context
	mCtx =
#ifdef ENV_OS_ANDROID
	nvgCreateGLES3
#else
	nvgCreateGL3
#endif 
		(NVG_ANTIALIAS | NVG_STENCIL_STROKES
#ifdef ENV_BUILD_DEBUG
		| NVG_DEBUG
#endif
		);

	mColorPrimary = rgba(0.5f, 0.5f, 0.5f, 0.9f);
	mColorSecondary = rgba(0.2f, 0.2f, 0.2f, 0.9f);

	nvgCreateFont(NVG, "basic", getFontPath());
}

Canvas::Val& Canvas::convert2pxls(Val& val) {
	val = val * mDpmm * mUIScale;
	return val;
}

Canvas::Val Canvas::as2pxls(Val val) {
	return val * mDpmm * mUIScale;
}

Canvas::Rect& Canvas::convert2pxls(Rect& rec) {
	convert2pxls(rec.x);
	convert2pxls(rec.y);
	convert2pxls(rec.z);
	convert2pxls(rec.w);
	return rec;
}

Canvas::Vec2& Canvas::convert2pxls(Vec2& vec) {
	convert2pxls(vec.x);
	convert2pxls(vec.y);
	return vec;
}

void Canvas::beginDraw(Rect viewport, tp::halnf dpmm, tp::halnf uiscale) {
	convert2pxls(viewport);
	nvgBeginFrame(NVG, viewport.size.x, viewport.size.y, 1);

	mVieport = { 0.f, 0.f, viewport.size.x, viewport.size.y };
	glViewport((GLsizei)mVieport.x, (GLsizei)mVieport.y, (GLsizei)mVieport.size.x, (GLsizei)mVieport.size.y);

	mDpmm = dpmm;
	mUIScale = tp::clamp(uiscale, 0.03f, 100.f);

	mClamping = mVieport;
}

void Canvas::resetViewport() {
	glViewport((GLsizei)mVieport.x, (GLsizei)mVieport.y, (GLsizei)mVieport.size.x, (GLsizei)mVieport.size.y);
}

void Canvas::clear(Col color) {
	glClearColor(color.r, color.g, color.b, color.a);
	//glClearDepth(0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Canvas::endDraw() {
	nvgEndFrame(NVG);
}

void Canvas::setClamping(Rect rec) {
	mClamping = rec;
	convert2pxls(rec);
	nvgScissor(NVG, rec.x, rec.y, rec.z, rec.w);
}

void Canvas::setCol1(const Col& col) {
	mColorPrimary = col;
}

void Canvas::setCol2(const Col& col) {
	mColorSecondary = col;
}

void Canvas::rect(Rect rec, halnf rounding, halnf borders) {
	convert2pxls(rec);
	convert2pxls(rounding);
	convert2pxls(borders);

	if (borders) {
		nvgBeginPath(NVG);

		auto borders_rec = rec;
		rec.pos += borders;
		rec.size -= borders * 2;

		if (rounding) {
			nvgRoundedRect(NVG, borders_rec.x, borders_rec.y, borders_rec.z, borders_rec.w, rounding);
		}
		else {
			nvgRect(NVG, borders_rec.x, borders_rec.y, borders_rec.z, borders_rec.w);
		}

		nvgFillColor(NVG, nvgRGBAf(mColorSecondary.r, mColorSecondary.g, mColorSecondary.b, mColorSecondary.a));
		nvgFill(NVG);
	}

	nvgBeginPath(NVG);

	if (rounding) {
		nvgRoundedRect(NVG, rec.x, rec.y, rec.z, rec.w, rounding);
	}
	else {
		nvgRect(NVG, rec.x, rec.y, rec.z, rec.w);
	}
	nvgFillColor(NVG, nvgRGBAf(mColorPrimary.r, mColorPrimary.g, mColorPrimary.b, mColorPrimary.a));
	nvgFill(NVG);
}

void Canvas::line(Vec2 p1, Vec2 p2, halnf thikness) {
	convert2pxls(p1);
	convert2pxls(p2);
	convert2pxls(thikness);

	nvgBeginPath(NVG);

	nvgMoveTo(NVG, p1.x, p1.y);
	nvgLineTo(NVG, p2.x, p2.y);

	nvgStrokeColor(NVG, nvgRGBAf(mColorPrimary.r, mColorPrimary.g, mColorPrimary.b, mColorPrimary.a));
	nvgStrokeWidth(NVG, thikness);
	nvgStroke(NVG);
}

void Canvas::trig(Vec2 p1, Vec2 p2, Vec2 p3) {
	convert2pxls(p1);
	convert2pxls(p2);
	convert2pxls(p3);

	nvgBeginPath(NVG);

	nvgMoveTo(NVG, p1.x, p1.y);
	nvgLineTo(NVG, p2.x, p2.y);
	nvgLineTo(NVG, p3.x, p3.y);

	nvgFillColor(NVG, nvgRGBAf(mColorPrimary.r, mColorPrimary.g, mColorPrimary.b, mColorPrimary.a));
	nvgFill(NVG);
}

void Canvas::circle(Vec2 rec, halnf radius, halnf borders) {
	convert2pxls(rec);
	convert2pxls(radius);
	convert2pxls(borders);
	DBG_BREAK(1);
}

void Canvas::text(const char* start, const char* end, Rect rec, halnf size, Align align) {
	convert2pxls(rec);
	convert2pxls(size);

	nvgFillColor(NVG, nvgRGBAf(mColorPrimary.r, mColorPrimary.g, mColorPrimary.b, mColorPrimary.a));
	nvgFontSize(NVG, size);
	nvgTextAlign(NVG, NVGalign(align));
	auto y = (align & Align::MIDDLE) ? rec.y + rec.w / 2 : rec.y;
	auto x = (align & Align::CENTER) ? rec.x + rec.z / 2 : rec.x;
	nvgText(NVG, x, y, start, end);
}

void Canvas::text(const string text, Rect rec, halnf size, Align align) {
	convert2pxls(rec);
	convert2pxls(size);

	nvgFillColor(NVG, nvgRGBAf(mColorPrimary.r, mColorPrimary.g, mColorPrimary.b, mColorPrimary.a));
	nvgFontSize(NVG, size);
	nvgTextAlign(NVG, NVGalign(align));

	//nvgTextBounds(NVG, );

	auto t_mid = rec.y + rec.w / 2;
	auto t_top = rec.y;
	auto res = bool(align & Align::MIDDLE);
	auto y = res ? t_mid : t_top;

	auto x_mid = rec.x + rec.z / 2;
	auto x_left = rec.x;
	res = bool(align & Align::CENTER);
	auto x = res ? x_mid : x_left;

	nvgText(NVG, x, y, text.cstr(), text.cstr() + text.size());
}


void Canvas::ImageHandle::createFromBuff(glw::fbuffer* buff, Canvas* drawer) {
	this->~ImageHandle();
	auto const size = buff->getSize();

#ifdef ENV_OS_ANDROID
	mId = nvglCreateImageFromHandleGLES3((NVGcontext*)drawer->mCtx, buff->texId(), size.x, size.y, 0);
#else
	mId = nvglCreateImageFromHandleGL3((NVGcontext*)drawer->mCtx, buff->texId(), (halni) size.x, (halni) size.y, 0);
#endif
}

void Canvas::ImageHandle::free(Canvas* drawer) {
	if (mId && (NVGcontext*)drawer->mCtx) {
		nvgDeleteImage((NVGcontext*)drawer->mCtx, mId);
	}
}

Canvas::ImageHandle::~ImageHandle() {}

void Canvas::drawImage(Rect rec, ImageHandle* image, halnf angle, halnf alpha, halnf rounding) {
	convert2pxls(rec);
	convert2pxls(rounding);


	auto imgPaint = nvgImagePattern(NVG, rec.x, rec.y, rec.z, rec.w, angle, image->mId, alpha);
	nvgBeginPath(NVG);
	nvgRoundedRect(NVG, rec.x, rec.y, rec.z, rec.w, rounding);
	nvgFillPaint(NVG, imgPaint);
	nvgFill(NVG);
}

void Canvas::drawColorwheel(Rect rec, const rgb& col) {
	convert2pxls(rec);

	float const x = rec.x;
	float const y = rec.y;
	float const w = rec.z;
	float const h = rec.w;

	hsv hsv = tp::hsv(col);
	float const hue = hsv.h / (NVG_PI * 2);

	int i;
	float r0, r1, ax, ay, bx, by, cx, cy, aeps, r;
	NVGpaint paint;

	nvgSave(NVG);

	cx = x + w * 0.5f;
	cy = y + h * 0.5f;
	r1 = (w < h ? w : h) * 0.5f - as2pxls(1.0f);
	r0 = r1 - as2pxls(3.0f);
	aeps = 0.5f / r1;	// half a pixel arc length in radians (2pi cancels out).

	for (i = 0; i < 6; i++) {
		float a0 = (float)i / 6.0f * NVG_PI * 2.0f - aeps;
		float a1 = (float)(i + 1.0f) / 6.0f * NVG_PI * 2.0f + aeps;
		nvgBeginPath(NVG);
		nvgArc(NVG, cx, cy, r0, a0, a1, NVG_CW);
		nvgArc(NVG, cx, cy, r1, a1, a0, NVG_CCW);
		nvgClosePath(NVG);
		ax = cx + cosf(a0) * (r0 + r1) * 0.5f;
		ay = cy + sinf(a0) * (r0 + r1) * 0.5f;
		bx = cx + cosf(a1) * (r0 + r1) * 0.5f;
		by = cy + sinf(a1) * (r0 + r1) * 0.5f;
		paint = nvgLinearGradient(NVG, ax, ay, bx, by, nvgHSLA(a0 / (NVG_PI * 2), 1.0f, 0.55f, 255), nvgHSLA(a1 / (NVG_PI * 2), 1.0f, 0.55f, 255));
		nvgFillPaint(NVG, paint);
		nvgFill(NVG);
	}

	nvgBeginPath(NVG);
	nvgCircle(NVG, cx, cy, r0 - 0.5f);
	nvgCircle(NVG, cx, cy, r1 + 0.5f);
	nvgStrokeColor(NVG, nvgRGBA(0, 0, 0, 64));
	nvgStrokeWidth(NVG, 1.0f);
	nvgStroke(NVG);

	// Selector
	nvgSave(NVG);
	nvgTranslate(NVG, cx, cy);
	nvgRotate(NVG, hue * NVG_PI * 2);

	// Marker on
	nvgStrokeWidth(NVG, 2.0f);
	nvgBeginPath(NVG);
	nvgRect(NVG, r0 - 1, -3, r1 - r0 + 2, 6);
	nvgStrokeColor(NVG, nvgRGBA(255, 255, 255, 192));
	nvgStroke(NVG);

	paint = nvgBoxGradient(NVG, r0 - 3, -5, r1 - r0 + 6, 10, 2, 4, nvgRGBA(0, 0, 0, 128), nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(NVG);
	nvgRect(NVG, r0 - 2 - 10, -4 - 10, r1 - r0 + 4 + 20, 8 + 20);
	nvgRect(NVG, r0 - 2, -4, r1 - r0 + 4, 8);
	nvgPathWinding(NVG, NVG_HOLE);
	nvgFillPaint(NVG, paint);
	nvgFill(NVG);

	// Center triangle
	r = r0 - 6;
	ax = cosf(120.0f / 180.0f * NVG_PI) * r;
	ay = sinf(120.0f / 180.0f * NVG_PI) * r;
	bx = cosf(-120.0f / 180.0f * NVG_PI) * r;
	by = sinf(-120.0f / 180.0f * NVG_PI) * r;
	nvgBeginPath(NVG);
	nvgMoveTo(NVG, r, 0);
	nvgLineTo(NVG, ax, ay);
	nvgLineTo(NVG, bx, by);
	nvgClosePath(NVG);
	paint = nvgLinearGradient(NVG, r, 0, ax, ay, nvgHSLA(hue, 1.0f, 0.5f, 255), nvgRGBA(255, 255, 255, 255));
	nvgFillPaint(NVG, paint);
	nvgFill(NVG);
	paint = nvgLinearGradient(NVG, (r + ax) * 0.5f, (0 + ay) * 0.5f, bx, by, nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255));
	nvgFillPaint(NVG, paint);
	nvgFill(NVG);
	nvgStrokeColor(NVG, nvgRGBA(0, 0, 0, 64));
	nvgStroke(NVG);

	// Select circle on triangle
	auto inner = as2pxls(3);
	//auto outter = as2pxls(5);

	float yt = hsv.v * hsv.s;
	float xt = hsv.v - 0.5f * yt;
	ay = sinf(120.0f / 180.0f * NVG_PI) * r * (-1.0f + xt * 2.0f);
	ax = cosf(120.0f / 180.0f * NVG_PI) * r * (1.0f - yt * 3.f);
	nvgStrokeWidth(NVG, as2pxls(0.6f));
	nvgBeginPath(NVG);
	nvgCircle(NVG, ax, ay, inner);
	nvgStrokeColor(NVG, nvgRGBA(255, 255, 255, 192));
	nvgStroke(NVG);

	//paint = nvgRadialGradient(NVG, ax, ay, 4, 9, nvgRGBA(0, 0, 0, 64), nvgRGBA(0, 0, 0, 0));
	//nvgBeginPath(NVG);
	//nvgRect(NVG, ax - outter, ay - outter, outter * 2, outter * 2);
	//nvgCircle(NVG, ax, ay, outter);
	//nvgPathWinding(NVG, NVG_HOLE);
	//nvgFillPaint(NVG, paint);
	//nvgFill(NVG);

	nvgRestore(NVG);

	nvgRestore(NVG);
}


Canvas::~Canvas() {
	if (NVG) {
#ifdef ENV_OS_ANDROID
		nvgDeleteGLES3(NVG);
#else
		nvgDeleteGL3(NVG);
#endif
	}
	mCtx = NULL;
}
