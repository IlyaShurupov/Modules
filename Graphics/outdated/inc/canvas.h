
#pragma once

#include "stringt.h"
#include "fbuffer.h"

namespace tp {
	namespace glw {
		class Canvas {

			typedef rectf Rect;
			typedef vec2f Vec2;
			typedef rgba Col;
			typedef tp::halnf Val;

			Col mColorPrimary;
			Col mColorSecondary;

			void* mCtx = NULL;
			tp::halnf mDpmm = 1;
			tp::halnf mUIScale = 1;

		public:

			Rect mClamping;
			Rect mVieport;

			enum Align : uint4 {
				LEFT = 1 << 0,	// Default, horizontally to left.
				CENTER = 1 << 1,	// horizontally to center.
				RIGHT = 1 << 2,	// horizontally to right.
				TOP = 1 << 3,	// vertically to top.
				MIDDLE = 1 << 4,	// vertically to middle.
				BOTTOM = 1 << 5,	// vertically to bottom.
				BASELINE = 1 << 6, // Default, vertically to baseline.
				CENTER_MIDDLE = CENTER | MIDDLE,
				LEFT_MIDDLE = LEFT | MIDDLE,
			};

			Canvas();

			Val& convert2pxls(Val&);
			Rect& convert2pxls(Rect&);
			Vec2& convert2pxls(Vec2&);
			Val as2pxls(Val);

			// Canvas Manipulation
			void beginDraw(Rect viewport, tp::halnf dpmm, tp::halnf uiscale);
			void clear(Col color = 0);
			void endDraw();

			void setClamping(Rect rec);
			void resetViewport();

			// Params control
			void setCol1(const Col& col);
			void setCol2(const Col& col);

			// Shapes
			void rect(Rect rec, halnf rounding = 0, halnf borders = 0);
			void circle(Vec2 rec, halnf radius, halnf borders = 0);
			void trig(Vec2 p1, Vec2 p2, Vec2 p3);
			void line(Vec2 p1, Vec2 p2, halnf thikness = 2);

			// Text
			void text(const string text, Rect rec, halnf size, Align align = Align::CENTER_MIDDLE);
			void text(const char* start, const char* end, Rect rec, halnf size, Align align = Align::CENTER_MIDDLE);

			void drawColorwheel(Rect rec, const rgb& col);

			// image
			struct ImageHandle {
				halni mId = NULL;
				void createFromBuff(glw::fbuffer* buff, Canvas* drawer);
				void free(Canvas* drawer);
				~ImageHandle();
			};

			void drawImage(Rect rec, ImageHandle* image, halnf angle = 0.f, halnf alpha = 1.f, halnf rounding = 0.f);

			~Canvas();
		};
	};
};