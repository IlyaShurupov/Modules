
#pragma once

#include "common.h"
#include "vec.h"
#include "rect.h"
#include "color.h"
#include "stringt.h"
#include "array.h"
#include "map.h"

#include "keycodes.h"
#include "queue.h"

namespace tp {

	extern tp::ModuleManifest gModuleGlw;

	namespace glw {

		struct PlatformContext;

		class Window {

		public:

			struct Device {
				time_ms FrameRate = NULL; // monitor fps
				vec2f Size; // size of the monitor
				tp::halnf mDPMM = 1;
			};

			struct Events {
				tp::Queue<KeyEvent> mKeysQueue;
				tp::uhalni mRedraw = 2;
				bool mClose = false;

				vec2f mCursor = 0;
				halnf mPressure = 1.f;

				vec2f mCursorPrev = 0.f;
				halnf mPressurePrev = 1.f;
			};

			struct Appearence {
				vec2f mPos = { 100.f, 100.f }; // global position
				vec2f mSize = { 300.f, 300.f }; // width and height of window
				vec2f mSizeMin = { 100.f, 100.f }; // min size possiable
				vec2f mSizeMax = { 700.f, 700.f }; // max size possiable

				bool mHiden = true;
				bool mMaximized = false;
				bool mMinimized = false;
				bool mTopZ = false;
				bool mAllDesktops = false;

				rgba mResizeColor = rgba(0.13f, 0.13f, 0.13f, 0.9f);

				rectf mMinMaxBox;

				tp::Array<rectf> mCaptionsAddArea;
				tp::Array<rectf> mCaptionsRemoveArea;
			};

		public:

			PlatformContext* mPlatformCtx = NULL;
			void platformCallback();
			void* platform_window() const;

			struct NativeEventListenerCallback {
				typedef void (call_back_func)(PlatformContext* ctx, void* cd);
				call_back_func* exec_begin = NULL;
				call_back_func* exec = NULL;
				call_back_func* exec_end = NULL;
				void* cd = NULL;
			};

			tp::HashMap<NativeEventListenerCallback, string> mNativeEventListeners;

		private:

			struct Cache {
				Appearence mAppearencePrev;
				vec2f mWindowSize = 0;
				rectf mRectBeforResizing = 0;
				bool mUserResizing = false;
				bool mMinMaxHover = false;
				bool mMouseTracked = false;
			} mCache;

			void applyAppearance();
			void initialize();

		public:

			static Device mDevice;
			Appearence mAppearence;
			Events mEvents;

			Window();
			Window(const Appearence& aAppearence);

			void pollEvents();
			void beginDraw();
			void endDraw();

			alni sizeAllocatedMem();
			alni sizeUsedMem();

			~Window();
		};
	};
};