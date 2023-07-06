
#include "animations.h"
#include "timer.h"

using namespace tp;
using namespace glw;

bool tp::glw::gInTransition = false;

halnf AnimValue::interpolate() const {
	if (!mTimeAnim) {
		return mVal;
	}

	auto dt = gCurrentTime - mTimeStart;
	if (dt > mTimeAnim) dt = mTimeAnim;
	auto t = (halnf)dt / mTimeAnim;
	t = (halnf)(0.511 + atan((t - 0.36) * 28) / 2.9);
	t = clamp(t, 0.f, 1.f);
	auto out = mValPrev + (mVal - mValPrev) * t;
	return out;
}

AnimValue::AnimValue() {}

void AnimValue::setAnimTime(halni time) {
	mTimeAnim = time;
}

AnimValue::AnimValue(halnf val) {
	mVal = val;
	mValPrev = mVal;
	mTimeStart = gCurrentTime;
}

bool AnimValue::inTransition() const {
	auto time_pased = gCurrentTime - mTimeStart >= mTimeAnim;
	return !time_pased;
}

void AnimValue::set(halnf val) {
	if (!inTransition()) {
		mValPrev = mVal;
	}

	if (val == mVal) {
		return;
	}

	mValPrev = get();
	mVal = val;

	if (!inTransition()) {
		mTimeStart = (halni)gCurrentTime;
	}
}

halnf AnimValue::getTarget() const {
	return mVal;
}

void AnimValue::setNoTransition(halnf val) {
	mValPrev = val;
	mVal = val;
	mTimeStart -= mTimeStart;
}

halnf AnimValue::get() const {
	if (inTransition()) {
		gInTransition = true;
	}
	return interpolate();
}

AnimValue::operator halnf() const {
	return get();
}

rectf AnimRect::get() const {
	return { x.get(), y.get() , z.get(), w.get() };
}

rectf AnimRect::getTarget() const {
	return { x.getTarget(), y.getTarget() , z.getTarget(), w.getTarget() };
}

void AnimRect::setNoTransition(rectf rec) {
	x.setNoTransition(rec.x);
	y.setNoTransition(rec.y);
	z.setNoTransition(rec.z);
	w.setNoTransition(rec.w);
}

void AnimRect::setAnimTime(const halni time) {
	x.setAnimTime(time);
	y.setAnimTime(time);
	z.setAnimTime(time);
	w.setAnimTime(time);
}

void AnimRect::set(const rectf& in) {
	x.set(in.x);
	y.set(in.y);
	z.set(in.z);
	w.set(in.w);
}

rgba AnimColor::get() const {
	auto col = mColor.get();
	return rgba(col.x, col.y, col.z, col.w);
}

void AnimColor::set(const rgba& col) {
	mColor.set(rectf(col.r, col.g, col.b, col.a));
}