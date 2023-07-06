
#include "SimpleGui.h"
#include "debugui.h"
#include "texture.h"
#include "timer.h"
#include "imgui.h"

struct CollorWheelWidget {
	tp::rgba col = { 0.3f, 0.3f, 0.3f, 0.9f };
	tp::rectf rec = { 10, 30, 40, 40 };

	void draw(tp::glw::Canvas& canvas) {
		canvas.setCol1(col);
		canvas.rect(rec, 3.f);
		canvas.drawColorwheel(rec, col.rgbs);
	}
};

struct Test {

	tp::glw::Window window;
	tp::glw::DebugUI ui;
	tp::glw::Canvas canvas;
	tp::glw::WindowsHeaderWidget header;
	tp::glw::WindowSimpleInputs inputs;
	tp::halnf mHeaderHeight = 6.5f;

	CollorWheelWidget colorwheel;
	tp::halnf uiscale = 1.f;
	tp::Timer time;
	tp::halni frames = 0;
	tp::halni fps = 0;

	Test() : window(), ui(window, debuguiCallBack, this), time(1000) {
		tp::glw::texture::init();
		canvas.setCol1({ 0.5f, 0.5f, 0.5f, 0.5f });
		window.mAppearence.mHiden = false;
	}

	void proc() {
		window.pollEvents();
		inputs.update(window, uiscale);

		header.header_rec = { (inputs.mWindowSizeMM.x - 60.f) / 2.f, 3.f, 60.f, mHeaderHeight };
		header.proc(window, inputs);
	}

	void draw() {
		// window frame
		window.beginDraw(); {
			// canvas draw
			canvas.beginDraw({ { 0, 0 },  inputs.mWindowSizeMM }, window.mDevice.mDPMM, uiscale); {
				canvas.clear();
				header.draw(canvas);
				colorwheel.draw(canvas);
			}	canvas.endDraw();
			// debug draw
			ui.drawDebugUI(window.mDevice.mDPMM);
		}	window.endDraw();
	}

	void run() {
		while (!window.mEvents.mClose) {
			proc();
			if (window.mEvents.mRedraw || tp::glw::gInTransition) {
				draw();
			}
			if (time.isTimeout()) {
				fps = frames;
				frames = 0;
				time.reset();
			}
			frames++;
		}
	}

	void debugui() {
		static char buff[100];
		ImGui::DragFloat("UI SCale", &uiscale, 0.02);
		ImGui::Text("Fps: %i", fps);
		ImGui::InputText("Fps: %i", buff, 100);
	}

	static void debuguiCallBack(void* self) {
		((Test*)self)->debugui();
	}

	~Test() {
		tp::glw::texture::deinit();
	}
};

CROSSPLATFORM_MAIN 
int main() {
	tp::ModuleManifest* ModuleDependencies[] = { &tp::gModuleGlw, NULL };
	tp::ModuleManifest TestModule("Test", NULL, NULL, ModuleDependencies);
	if (!TestModule.initialize()) {
		return 1;
	}

	{
		Test test;
		test.run();
	}

	TestModule.deinitialize();
}