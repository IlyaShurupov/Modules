
#include "window.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <stdlib.h>

tp::glw::Window::Device tp::glw::Window::mDevice;

static void glfw_error_callback(int error, const char* description) {
	std::cout << stderr << "Glfw Error" << error << " " << description << "\n";
}

void tp::glw::Window::init() {
	tp::glw::Window::mDevice.Size;
	tp::glw::Window::mDevice.FrameRate;

	// Setup window

	setenv("DISPLAY", ":0.0", 0); // does overwrite

	glfwSetErrorCallback(glfw_error_callback);
	RelAssert(glfwInit());


	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	auto mode = glfwGetVideoMode(primary);

	tp::glw::Window::mDevice.Size = { mode->width, mode->height };
	tp::glw::Window::mDevice.FrameRate = mode->refreshRate;
}

#define MAP_EV(SYSKEY, code) case SYSKEY: { self->mEvents.mKeysQueue.push(tp::KeyEvent{ code, state }); break; }

namespace tp {
	namespace glw {
		struct PlatformContext {

			GLFWwindow* window;
			tp::glw::Window* window_ctx = NULL;

			PlatformContext(tp::glw::Window* a_window_ctx) {

				window_ctx = a_window_ctx;

				// Create window with graphics context
				window = glfwCreateWindow(300, 300, " ", NULL, NULL);
				RelAssert(window);

				glfwMakeContextCurrent(window);

				GLenum err = glewInit();
				if (GLEW_OK != err)
				{
					std::cout << stderr << "Error:" << glewGetErrorString(err);
					RelAssert(0);
				}

				glfwSwapInterval(1); // Enable vsync

				glfwSetWindowUserPointer(window, window_ctx);

				glfwSetKeyCallback(window, key_callback);
				glfwSetMouseButtonCallback(window, mouse_button_callback);
				glfwSetScrollCallback(window, scroll_callback);
			}

			void applyAppearance(tp::glw::Window::Appearence& appear, tp::glw::Window::Appearence& prev, tp::glw::Window* win_ctx) {

				if (!(appear.mSize == prev.mSize)) {
					glfwSetWindowSize(window, appear.mSize.x, appear.mSize.y);
					prev.mSize = appear.mSize;
				}

				if (!(appear.mPos == prev.mPos)) {
					//glfwSetWindowPos(window, appear.mPos.x, appear.mPos.y);

					win_ctx->mEvents.mCursor -= appear.mPos - prev.mPos;
					//win_ctx->mEvents.mCursorPrev -= appear.mPos - prev.mPos;
					prev.mPos = appear.mPos;
				}

				if (!(appear.mSizeMax == prev.mSizeMax && appear.mSizeMin == prev.mSizeMin)) {
					glfwSetWindowSizeLimits(window, appear.mSizeMin.x, appear.mSizeMin.y, appear.mSizeMax.x, appear.mSizeMax.y);
					prev.mSizeMax = appear.mSizeMax;
					prev.mSizeMin = appear.mSizeMin;
				}

				if (!(appear.mMaximized == prev.mMaximized)) {
					DBG_BREAK(1);
				}

				if (!(appear.mMinimized == prev.mMinimized)) {
					DBG_BREAK(1);
				}

				int width, height;
				glfwGetWindowSize(window, &width, &height);
				appear.mSize = { width, height };
				prev.mSize = appear.mSize;

				glfwGetWindowPos(window, &width, &height);
				appear.mPos = { width, height };
				prev.mPos = appear.mPos;
			}

			void beginDraw() {
				//int display_w, display_h;
				//glfwGetFramebufferSize(window, &display_w, &display_h);
				//glViewport(0, 0, display_w, display_h);
				//glClearColor(0, 0, 0, 0);
				//glClear(GL_COLOR_BUFFER_BIT);
			}

			void endDraw() {
				glfwSwapBuffers(window);
			}

			void pollEvents() {

				glfwPollEvents();

				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				window_ctx->mEvents.mCursorPrev = window_ctx->mEvents.mCursor;
				window_ctx->mEvents.mCursor = { xpos, ypos };
			}

			static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
				auto self = (tp::glw::Window*) glfwGetWindowUserPointer(window);

				if (action != GLFW_PRESS && action != GLFW_RELEASE) {
					return;
				}

				auto state = (action == GLFW_RELEASE) ? tp::KeyEvent::EventState::RELEASED : tp::KeyEvent::EventState::PRESSED;

				//* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
				if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
					self->mEvents.mKeysQueue.push(tp::KeyEvent{ tp::Keycode(key), state });
					return;
				}

				//* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
				else if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
					self->mEvents.mKeysQueue.push(tp::KeyEvent{ tp::Keycode(key), state });
					return;
				}

				switch (key) {
					MAP_EV(GLFW_KEY_LEFT_SHIFT, tp::Keycode::LEFT_SHIFT);
					MAP_EV(GLFW_KEY_LEFT, tp::Keycode::LEFT);
					MAP_EV(GLFW_KEY_RIGHT, tp::Keycode::RIGHT);
					MAP_EV(GLFW_KEY_UP, tp::Keycode::UP);
					MAP_EV(GLFW_KEY_DOWN, tp::Keycode::DOWN);
					MAP_EV(GLFW_KEY_BACKSPACE, tp::Keycode::BACKSPACE);
					MAP_EV(GLFW_KEY_LEFT_CONTROL, tp::Keycode::LEFT_CONTROL);
					MAP_EV(GLFW_KEY_MENU, tp::Keycode::LEFT_ALT);
					MAP_EV(GLFW_KEY_ENTER, tp::Keycode::ENTER);
					MAP_EV(GLFW_KEY_SPACE, tp::Keycode::SPACE);
					MAP_EV(GLFW_KEY_COMMA, tp::Keycode::COMMA);
					MAP_EV(GLFW_KEY_PERIOD, tp::Keycode::PERIOD);
					MAP_EV(GLFW_KEY_TAB, tp::Keycode::TAB);
					MAP_EV(GLFW_KEY_EQUAL, tp::Keycode::EQUAL);
					MAP_EV(GLFW_KEY_MINUS, tp::Keycode::MINUS);
					MAP_EV(GLFW_KEY_3, tp::Keycode::TILDA);
					MAP_EV(GLFW_KEY_4, tp::Keycode::BRA);
					MAP_EV(GLFW_KEY_5, tp::Keycode::SLASH);
					MAP_EV(GLFW_KEY_6, tp::Keycode::KET);
					MAP_EV(GLFW_KEY_1, tp::Keycode::DOUBLE_DOT);
					MAP_EV(GLFW_KEY_7, tp::Keycode::QUOTES);
					MAP_EV(GLFW_KEY_2, tp::Keycode::INV_SLASH);
					MAP_EV(GLFW_KEY_ESCAPE, tp::Keycode::ESCAPE);
				}
			}

			static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
				auto self = (tp::glw::Window*)glfwGetWindowUserPointer(window);

				if (action != GLFW_PRESS && action != GLFW_RELEASE) {
					return;
				}

				auto state = (action == GLFW_RELEASE) ? tp::KeyEvent::EventState::RELEASED : tp::KeyEvent::EventState::PRESSED;


				switch (button) {
					case GLFW_MOUSE_BUTTON_RIGHT: {
						self->mEvents.mKeysQueue.push(tp::KeyEvent{ tp::Keycode::MOUSE2, state });
					} break;
					case GLFW_MOUSE_BUTTON_LEFT: {
						self->mEvents.mKeysQueue.push(tp::KeyEvent{ tp::Keycode::MOUSE1, state });
					} break;
					case GLFW_MOUSE_BUTTON_MIDDLE: {
						self->mEvents.mKeysQueue.push(tp::KeyEvent{ tp::Keycode::MOUSE3, state });
					} break;
				}
			}

			static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
			}

			~PlatformContext() {
				glfwDestroyWindow(window);
			}
		};
	};
};


void tp::glw::Window::deinit() {}

tp::glw::Window::Window() {
	initialize();

	mAppearence.mSize = tp::glw::Window::mDevice.Size / 1.5f;
	mAppearence.mPos = (tp::glw::Window::mDevice.Size - mAppearence.mSize) / 2.f;
	mAppearence.mSizeMax = tp::glw::Window::mDevice.Size;
	
	applyAppearance();
}

tp::glw::Window::Window(const Appearence& aAppearence) {
	mAppearence = aAppearence;
	initialize();
}

void tp::glw::Window::initialize() {
	mPlatformCtx = new PlatformContext(this);
	applyAppearance();
}

void tp::glw::Window::applyAppearance() {
	mPlatformCtx->applyAppearance(mAppearence, this->mCache.mAppearencePrev, this);
}

void tp::glw::Window::pollEvents() {
	applyAppearance();
	mPlatformCtx->pollEvents();

	mEvents.mRedraw |= (mEvents.mCursorPrev - mEvents.mCursor) != 0;
	mEvents.mRedraw |= mEvents.mKeysQueue.length;
}

void tp::glw::Window::platformCallback() {}

void tp::glw::Window::beginDraw() {
	mPlatformCtx->beginDraw();
}

void tp::glw::Window::endDraw() {
	mPlatformCtx->endDraw();
	if (mEvents.mRedraw-- < 0) mEvents.mRedraw = 0;
}

void* tp::glw::Window::platform_window() const {
	return mPlatformCtx->window;
}

tp::alni tp::glw::Window::sizeAllocatedMem() {
	return 0;
}

tp::alni tp::glw::Window::sizeUsedMem() {
	return 0;
}

tp::glw::Window::~Window() {
	delete mPlatformCtx;
}
