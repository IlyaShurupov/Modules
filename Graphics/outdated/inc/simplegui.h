#pragma once

#include "window.h"
#include "canvas.h"
#include "animations.h"

namespace tp {
	namespace glw {

		class WindowSimpleInputs {
			enum class Mouse : uint1 { NONE, PRESSED, HOLD, RELEASED } mMouse;
			enum class Move : uint1 { NONE, LEFT, RIGHT, UP, DOWN } mMove;
		public:

			halnf mScaleVal = 0;
			vec2f mWindowSizeMM;
			vec2f mCrs;
			vec2f mCrsPrev;
			vec2f mCrsmDelta;
			halnf mPressure = 0.f;

			void update(const glw::Window& window, halnf uiscale);
			bool Activated();
			bool Anticipating();
			bool Selected();
			bool MoveLeft();
			bool MoveRight();
			bool MoveUp();
			bool MoveDown();
			bool Drag();
		};

		struct ButtonWidget {
			rectf mRect = { 0.f, 10.f };
			rgba mCol = { 0.3f, 0.3f, 0.3f, 1.f };
			bool mPressed = false;
			AnimRect mAnimRec;
			void draw(glw::Canvas& canvas);
			void proc(glw::WindowSimpleInputs& inputs);
		};

		struct CheckBoxWidget {
			rectf mRect = { 0.f, 10.f };
			rgba mCol = { 0.3f, 0.3f, 0.3f, 1.f };
			rgba mColActive = { 0.5f, 0.5f, 0.5f, 1.f };

			bool mValue = false;
			AnimRect mAnimRec;
			AnimColor mAnimCol;

			void draw(glw::Canvas& canvas);
			void proc(glw::WindowSimpleInputs& inputs);
		};

		struct WindowsHeaderWidget {
			rectf header_rec = { 10, 10, 50, 10 };
			rgba col = { 0.13f, 0.13f, 0.13f, 0.9f };
			rectf grab_rec = 0.f;

			ButtonWidget mExitButton;
			ButtonWidget mMaxButton;
			ButtonWidget mMinButton;
			CheckBoxWidget mPinButton;

			void updRecs();
			void draw(glw::Canvas& canvas);
			void proc(glw::Window& window, glw::WindowSimpleInputs& inputs);
		};

	};
};