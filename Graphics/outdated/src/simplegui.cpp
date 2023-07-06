#pragma once

#include "simplegui.h"

using namespace tp;
using namespace glw;

void WindowSimpleInputs::update(const glw::Window& window, halnf uiscale) {
	mScaleVal = (window.mDevice.mDPMM * uiscale);
	mWindowSizeMM = window.mAppearence.mSize / mScaleVal;
	mCrs = window.mEvents.mCursor / mScaleVal;
	mCrsPrev = window.mEvents.mCursorPrev / mScaleVal;
	mCrsmDelta = mCrs - mCrsPrev;

	if (mMouse == Mouse::PRESSED) {
		mMouse = Mouse::HOLD;
	}
	else if (mMouse == Mouse::RELEASED) {
		mMouse = Mouse::NONE;
	}
	mMove = Move::NONE;

	for (auto event : window.mEvents.mKeysQueue) {
		bool press = event.data().event_state == KeyEvent::EventState::PRESSED;
		if (press) {
			switch (event.data().code) {
			case Keycode::UP: mMove = Move::UP; break;
			case Keycode::DOWN: mMove = Move::DOWN; break;
			case Keycode::LEFT: mMove = Move::LEFT; break;
			case Keycode::RIGHT: mMove = Move::RIGHT; break;
			}
		}
		if (event.data().code == Keycode::MOUSE1) {
			if (press) {
				mMouse = Mouse::PRESSED;
			}
			else {
				mMouse = Mouse::RELEASED;
			}
		}
	}
}

bool WindowSimpleInputs::Activated() { return mMouse == Mouse::PRESSED; }
bool WindowSimpleInputs::Anticipating() { return mMouse == Mouse::HOLD; }
bool WindowSimpleInputs::Selected() { return mMouse == Mouse::RELEASED; }
bool WindowSimpleInputs::MoveLeft() { return mMove == Move::LEFT; }
bool WindowSimpleInputs::MoveRight() { return mMove == Move::RIGHT; }
bool WindowSimpleInputs::MoveUp() { return mMove == Move::UP; }
bool WindowSimpleInputs::MoveDown() { return mMove == Move::DOWN; }
bool WindowSimpleInputs::Drag() { return mCrsmDelta != 0; }

void ButtonWidget::draw(glw::Canvas& canvas) {
	canvas.setCol1(mCol);
	canvas.rect(mAnimRec.get(), 1.f);
}

void ButtonWidget::proc(glw::WindowSimpleInputs& inputs) {
	auto rec = mAnimRec.get();
	mPressed = false;

	mAnimRec.setAnimTime(100);

	if (rec.inside(inputs.mCrs)) {
		if (inputs.Anticipating()) {
			auto new_rec = mRect;
			new_rec.scale_from_center(0.3f, true);
			mAnimRec.setNoTransition(new_rec);
		}
		else {
			mAnimRec.set(mRect);
		}

		if (inputs.Selected()) {
			mPressed = true;
		}
	}
	else {
		auto new_rec = mRect;
		new_rec.scale_from_center(0.3f, true);
		mAnimRec.set(new_rec);
	}
}

void CheckBoxWidget::draw(glw::Canvas& canvas) {
	canvas.setCol1(mAnimCol.get());
	canvas.rect(mAnimRec.get(), 1.f);
}

void CheckBoxWidget::proc(glw::WindowSimpleInputs& inputs) {
	auto rec = mAnimRec.get();

	mAnimRec.setAnimTime(100);

	if (rec.inside(inputs.mCrs)) {
		if (inputs.Anticipating()) {
			auto new_rec = mRect;
			new_rec.scale_from_center(0.3f, true);
			mAnimRec.setNoTransition(new_rec);
		}
		else {
			mAnimRec.set(mRect);
		}

		if (inputs.Selected()) {
			mValue = !mValue;
		}
	}
	else {
		auto new_rec = mRect;
		new_rec.scale_from_center(0.3f, true);
		mAnimRec.set(new_rec);
	}

	if (mValue) {
		mAnimCol.set(mColActive);
	}
	else {
		mAnimCol.set(mCol);
	}
}

void WindowsHeaderWidget::updRecs() {
	auto padding = 0.7f;
	auto butt_size = header_rec.w - padding * 2.f;
	grab_rec = { header_rec.pos, { header_rec.z - header_rec.w * 4, header_rec.w } };
	mPinButton.mRect = { { grab_rec.pos.x + grab_rec.size.x + padding, grab_rec.y + padding }, butt_size };
	mMinButton.mRect = { { mPinButton.mRect.pos.x + mPinButton.mRect.size.x + padding * 2, mPinButton.mRect.y }, butt_size };
	mMaxButton.mRect = { { mMinButton.mRect.pos.x + mMinButton.mRect.size.x + padding * 2, mPinButton.mRect.y }, butt_size };
	mExitButton.mRect = { { mMaxButton.mRect.pos.x + mMaxButton.mRect.size.x + padding * 2, mPinButton.mRect.y }, butt_size };

	mPinButton.mCol = { 0.3f, 0.3f, 0.3f, 0.9f};
	mMinButton.mCol = { 0.3f, 0.3f, 0.8f, 0.9f };
	mMaxButton.mCol = { 0.3f, 0.8f, 0.3f, 0.9f };
	mExitButton.mCol = { 0.8f, 0.3f, 0.3f, 0.9f };
}

void WindowsHeaderWidget::draw(glw::Canvas& canvas) {
	updRecs();
	canvas.setCol1(col);
	canvas.rect(header_rec, 1.f);
	canvas.setCol1(1.f);
	canvas.text("MemLeaks", grab_rec, 4.f);

	mExitButton.draw(canvas);
	mMaxButton.draw(canvas);
	mMinButton.draw(canvas);
	mPinButton.draw(canvas);
}

void WindowsHeaderWidget::proc(glw::Window& window, glw::WindowSimpleInputs& inputs) {
	get_time();

	mExitButton.proc(inputs);
	mMaxButton.proc(inputs);
	mMinButton.proc(inputs);
	mPinButton.proc(inputs);

	window.mAppearence.mMinMaxBox = mMaxButton.mRect * inputs.mScaleVal;
	window.mAppearence.mCaptionsAddArea = { grab_rec * inputs.mScaleVal };
	window.mAppearence.mMinimized = mMinButton.mPressed;
	window.mEvents.mClose = mExitButton.mPressed;
	window.mAppearence.mTopZ = mPinButton.mValue;
}