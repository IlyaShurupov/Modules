
#include "window.h"

namespace ImGui {

	bool SubMenuBegin(const char* desc, int level);
	void SubMenuEnd(int level);

	void ToolTip(const char* desc);

	struct PopupData {
		bool opened = false;
		bool ishovered = false;
		tp::vec2f p1 = 0;
		tp::vec2f p2 = 0;
		operator bool() {
			return opened;
		}
	};

	PopupData HoverPopupBeginButton(const char* id, tp::vec2f butsize = 200, tp::vec2f popupsize = 200);
	PopupData HoverPopupBegin(const char* id, tp::vec2f size = 200, tp::vec2f pos = -1, int flags = 0);
	void HoverPopupEnd(PopupData& in);

	void ApplyStyle(tp::halnf dpmm, float font_size_mm = 5, float ui_scale = 1);
};

namespace tp {
	namespace glw {
		
		typedef void (DebugUiDrawCallBack)(void* cd);

		struct DebugUI {
			tp::halnf mDPMM = 1;
			tp::halnf mFontSizeMM = 3;
			tp::halnf mUIScale = 1;

			DebugUiDrawCallBack* cb = NULL;
			void* cd = NULL;

			struct DebugUiInternalContext* mCtx = NULL;

			DebugUI(Window& window, DebugUiDrawCallBack* acb, void* acd);
			void drawDebugUI(tp::halnf dpmm);
			~DebugUI();
		};
	};
};