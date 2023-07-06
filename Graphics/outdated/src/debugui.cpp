
#include "debugui.h"

#include "typelist.h"
#include "stringt.h"
#include "stack.h"
#include "npple.h"
#include "rect.h"
#include "map.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"

// -------------- Platform Includes ---------------------- // 

#if defined ENV_OS_WINDOWS
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_win32.h"
#include <Windows.h>
#elif defined ENV_OS_LINUX
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#elif defined ENV_OS_ANDROID
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_android.h"
#include <android/sensor.h>
#include <android/log.h>
#include  <android/input.h>
#include <string.h>
#endif


#if defined ENV_OS_WINDOWS
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern HWND windowsPlatformContextGetNativeWindow(tp::glw::PlatformContext* ctx);
extern UINT windowsPlatformContextGetMessage(tp::glw::PlatformContext* ctx);
extern WPARAM windowsPlatformContextGetWparam(tp::glw::PlatformContext* ctx);
extern LRESULT windowsPlatformContextGetLparam(tp::glw::PlatformContext* ctx);

#elif defined ENV_OS_LINUX
#elif defined ENV_OS_ANDROID
extern AInputEvent* androidPlatformContextGetEvent(tp::glw::PlatformContext* ctx);
extern const char* get_android_abs_path(const char* path, bool second = false);

const char* AndroidGetImGuiIniPath() {
	static char imgui_path[512];
	auto tmp = get_android_abs_path("imgui.ini");
	memcpy(imgui_path, tmp, strlen(tmp));
	return imgui_path;
}
#endif

static const char* getFontPath() {
#ifdef ENV_OS_ANDROID
	return get_android_abs_path("rsc/fonts/CONSOLAB.TTF");
#else
	return "./rsc/fonts/CONSOLAB.TTF";
#endif
}

namespace tp {
	namespace glw {
		struct DebugUiInternalContext {

			DebugUiDrawCallBack* cb = NULL;
			void* cd = NULL;

			ImGuiContext* ui_context = NULL;
			ImGuiContext* vk_context = NULL;

			bool initialized = false;

			bool ui_first_drawn = false;
			bool vk_first_drawn = false;

			bool ui_context_want_active = true;
			bool ui_context_want_active_vk = false;
			bool vk_context_want_active = false;

			tp::halnf dpmm;
			tp::halnf font_size_mm;
			tp::halnf ui_scale;

			struct VKState {

				struct InputButton {
					char val = '?';
					char shift_val = '?';
					tp::halnf width = 1;

					void (*action)(InputButton* button, VKState* state) = [](InputButton* button, VKState* state) {
						if (state->mShifted) {
							ImGui::GetIO().AddInputCharacter(button->shift_val);
						}
						else {
							ImGui::GetIO().AddInputCharacter(button->val);
						}
					};

					const char* display_name = NULL;
				};

				struct Config {
					InputButton** rows = NULL;
					tp::halni nrows = NULL;
					tp::halnf* rows_widths = NULL;
					tp::halnf* rows_sizes = NULL;
				};

				bool mShifted = false;

				static const tp::uhalni mNConfigs = 2;
				Config mConfigs[mNConfigs];
				tp::uhalni mActiveConfigIdx = 0;

				InputButton mCommonRow[7];
				//bool mActive = false;

				VKState() {
					mCommonRow[0] = { '^', '^', 1 , [](InputButton* button, VKState* state) { state->mShifted = !state->mShifted; } };

					mCommonRow[1] = { ' ', ' ', 1,
						[](InputButton* button, VKState* state) {
							state->mActiveConfigIdx++;
							if (state->mActiveConfigIdx == state->mNConfigs) {
								state->mActiveConfigIdx = 0;
							}
						},
						"..."
					};

					mCommonRow[2] = { ' ', ' ', 2 };

					mCommonRow[3] = { '<', '<', 2, [](InputButton* button, VKState* state) {
						ImGui::GetIO().AddKeyEvent(ImGuiKey_Backspace, true);
						ImGui::GetIO().AddKeyEvent(ImGuiKey_Backspace, false);
					} };

					mCommonRow[4] = { '.', '.', 1 };
					mCommonRow[5] = { '@', '@', 1 };

					mCommonRow[6] = { ' ', ' ', 2, [](InputButton* button, VKState* state) {
						ImGui::GetIO().AddKeyEvent(ImGuiKey_Enter, true);
						ImGui::GetIO().AddKeyEvent(ImGuiKey_Enter, false);
					},  "Entrer" };

					setupMainConfig(mConfigs[0]);
					setupAdditionalConfig(mConfigs[1]);
				}

				void setupMainConfig(Config& config) {

					static InputButton row1[] = {
						{'1', '!'},
						{'2', '@'},
						{'3', '#'},
						{'4', '$'},
						{'5', '%'},
						{'6', '^'},
						{'7', '&'},
						{'8', '*'},
						{'9', '('},
						{'0', ')'},
					};

					static InputButton row2[] = {
						{'q', 'Q'},
						{'w', 'W'},
						{'e', 'E'},
						{'r', 'R'},
						{'t', 'T'},
						{'y', 'Y'},
						{'u', 'U'},
						{'i', 'I'},
						{'o', 'O'},
						{'p', 'P'},
					};

					static InputButton row3[] = {
						{'a', 'A'},
						{'s', 'S'},
						{'d', 'D'},
						{'f', 'F'},
						{'g', 'G'},
						{'h', 'H'},
						{'j', 'J'},
						{'k', 'K'},
						{'l', 'L'},
					};

					static InputButton row4[] = {
						{'z', 'Z'},
						{'x', 'X'},
						{'c', 'C'},
						{'v', 'V'},
						{'b', 'B'},
						{'n', 'N'},
						{'m', 'M'},
					};

					static InputButton* rows[] = {
						row1,
						row2,
						row3,
						row4,
						mCommonRow,
					};

					static tp::halnf rows_sizes[IM_ARRAYSIZE(rows)] = {
						IM_ARRAYSIZE(row1),
						IM_ARRAYSIZE(row2),
						IM_ARRAYSIZE(row3),
						IM_ARRAYSIZE(row4),
						IM_ARRAYSIZE(mCommonRow),
					};

					static tp::halnf rows_widths[IM_ARRAYSIZE(rows)];
					for (auto row = 0; row < IM_ARRAYSIZE(rows); row++) {
						for (auto butt = 0; butt < rows_sizes[row]; butt++) {
							rows_widths[row] += rows[row][butt].width;
						}
					}

					config = {
						rows,
						IM_ARRAYSIZE(rows),
						rows_widths,
						rows_sizes,
					};
				}

				void setupAdditionalConfig(Config& config) {
					static InputButton row1[] = {
						{'~', '~'},
						{'`', '`'},
						{'_', '_'},
						{'|', '|'},
						{'\\', '\\'},
						{'/', '/'},
						{'"', '"'},
						{'\'', '\''},
					};

					static InputButton row2[] = {
						{';', ';'},
						{':', ':'},
						{'?', '?'},
						{',', ','},
						{'[', '['},
						{']', ']'},
						{'{', '{'},
						{'}', '}'},
						{'(', '('},
						{')', ')'},
					};

					static InputButton row3[] = {
						{'-', '-'},
						{'=', '='},
						{'*', '*'},
						{'+', '+'},
						{'<', '<'},
						{'>', '>'},
					};

					static InputButton row4[] = {
						{' ', ' ', 1, [](InputButton* butt, VKState* state) {
								ImGui::GetIO().AddKeyEvent(ImGuiKey_UpArrow, true);
								ImGui::GetIO().AddKeyEvent(ImGuiKey_UpArrow, false);
							}, "up" },
						{' ', ' ', 1, [](InputButton* butt, VKState* state) {
								ImGui::GetIO().AddKeyEvent(ImGuiKey_DownArrow, true);
								ImGui::GetIO().AddKeyEvent(ImGuiKey_DownArrow, false);
							}, "down"},
						{' ', ' ', 1, [](InputButton* butt, VKState* state) {
								ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftArrow, true);
								ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftArrow, false);
							}, "left"},
						{' ', ' ', 1, [](InputButton* butt, VKState* state) {
								ImGui::GetIO().AddKeyEvent(ImGuiKey_RightArrow, true);
								ImGui::GetIO().AddKeyEvent(ImGuiKey_RightArrow, false);
							}, "right"},
					};

					static InputButton* rows[] = {
						row1,
						row2,
						row3,
						row4,
						mCommonRow,
					};

					static tp::halnf rows_sizes[IM_ARRAYSIZE(rows)] = {
						IM_ARRAYSIZE(row1),
						IM_ARRAYSIZE(row2),
						IM_ARRAYSIZE(row3),
						IM_ARRAYSIZE(row4),
						IM_ARRAYSIZE(mCommonRow),
					};

					static tp::halnf rows_widths[IM_ARRAYSIZE(rows)];
					for (auto row = 0; row < IM_ARRAYSIZE(rows); row++) {
						for (auto butt = 0; butt < rows_sizes[row]; butt++) {
							rows_widths[row] += rows[row][butt].width;
						}
					}

					config = {
						rows,
						IM_ARRAYSIZE(rows),
						rows_widths,
						rows_sizes,
					};
				}

			} mVKState;

			DebugUiInternalContext(Window& window, DebugUiDrawCallBack* acb, void* acd) {

				cb = acb;
				cd = acd;

				ui_context = ImGui::CreateContext();
				ImGui::SetCurrentContext(ui_context);
				ImGuiContextInit(window);

				vk_context = ImGui::CreateContext();
				ImGui::SetCurrentContext(vk_context);
				ImGuiContextInit(window);

				initialized = true;
			}

			void drawDebugUI(tp::halnf a_dpmm, tp::halnf a_font_size_mm, tp::halnf a_ui_scale) {
				dpmm = a_dpmm;
				font_size_mm = a_font_size_mm;
				ui_scale = a_ui_scale;

				if (!initialized) return;

				ImGui::SetCurrentContext(ui_context);
				ImGui::ApplyStyle(dpmm, font_size_mm, ui_scale);
				ImGuiContextBeginFrame();
				cb(cd);
				ImGuiContextEndFrame();
				ImGui::SetCurrentContext(NULL);
				ui_first_drawn = true;

				if (ui_context_want_active_vk || vk_context_want_active) {
					ImGui::SetCurrentContext(vk_context);
					ImGui::ApplyStyle(dpmm, font_size_mm, ui_scale);
					ImGuiContextBeginFrame();
					VirtualKeyboard(dpmm, font_size_mm, ui_scale);
					ImGuiContextEndFrame();
					ImGui::SetCurrentContext(NULL);

					vk_first_drawn = true;
				}
			}

			~DebugUiInternalContext() {
				if (!initialized) return;

				ImGui::SetCurrentContext(ui_context);
				ImGuiContextDeinit();
				ImGui::DestroyContext(ui_context);

				ImGui::SetCurrentContext(vk_context);
				ImGuiContextDeinit();
				ImGui::DestroyContext(vk_context);
			}

		private:

			void ImGuiContextInit(Window& window) {
#if defined ENV_OS_WINDOWS
				ImGui_ImplOpenGL3_Init();
				ImGui_ImplWin32_Init(window.platform_window());
				window.mNativeEventListeners.put("imgui", {
					nativeWindowEventListenerWrapBegin,
					nativeWindowEventListenerWrap,
					nativeWindowEventListenerWrapEnd,
					this
					});
#elif defined ENV_OS_LINUX
				ImGui_ImplOpenGL3_Init();
				ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window.platform_window(), true);
#elif defined ENV_OS_ANDROID
				ImGui_ImplOpenGL3_Init("#version 300 es");
				ImGui_ImplAndroid_Init((ANativeWindow*)window.platform_window());
				ImGui::GetIO().IniFilename = AndroidGetImGuiIniPath();
				window.mNativeEventListeners.put("imgui", {
					nativeWindowEventListenerWrapBegin,
					nativeWindowEventListenerWrap,
					nativeWindowEventListenerWrapEnd,
					this
					});
#endif
			}

			void ImGuiContextBeginFrame() {
#if defined ENV_OS_WINDOWS
				ImGui_ImplWin32_NewFrame();
				ImGui_ImplOpenGL3_NewFrame();
#elif defined ENV_OS_LINUX
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
#elif defined ENV_OS_ANDROID
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplAndroid_NewFrame();
#endif
				ImGui::NewFrame();
			}

			void ImGuiContextEndFrame() {
				ImGui::Render();

#if defined ENV_OS_WINDOWS
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#elif defined ENV_OS_LINUX
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#elif defined ENV_OS_ANDROID
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
			}

			void ImGuiContextDeinit() {
#if defined ENV_OS_WINDOWS
				ImGui_ImplWin32_Shutdown();
#elif defined ENV_OS_LINUX
				ImGui_ImplGlfw_Shutdown();
#elif defined ENV_OS_ANDROID
				ImGui_ImplAndroid_Shutdown();
#endif
				ImGui_ImplOpenGL3_Shutdown();
			}

			void nativeWindowEventListenerPlatform(tp::glw::PlatformContext* ctx) {
#if defined ENV_OS_WINDOWS
				auto native_window = windowsPlatformContextGetNativeWindow(ctx);
				auto message = windowsPlatformContextGetMessage(ctx);
				auto w_param = windowsPlatformContextGetWparam(ctx);
				auto l_param = windowsPlatformContextGetLparam(ctx);
				// IMGUI : needs to be commented out
				//if (bd->MouseButtonsDown == 0 && ::GetCapture() == nullptr)
				//    ::SetCapture(hwnd);
				ImGui_ImplWin32_WndProcHandler(native_window, message, w_param, l_param);
#elif defined ENV_OS_LINUX
#elif defined ENV_OS_ANDROID

				auto aev = androidPlatformContextGetEvent(ctx);

				int32_t event_type = AInputEvent_getType(aev);

				ImGui_ImplAndroid_HandleInputEvent(aev);

				auto& io = ImGui::GetIO();

				if (event_type == AINPUT_EVENT_TYPE_MOTION); {
					int32_t event_action = AMotionEvent_getAction(aev);
					int32_t event_pointer_index = (event_action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
					event_action &= AMOTION_EVENT_ACTION_MASK;

					if ((AMotionEvent_getToolType(aev, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_FINGER)
						|| (AMotionEvent_getToolType(aev, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_UNKNOWN)) {

						if (event_action == AMOTION_EVENT_ACTION_DOWN) {
							io.AddMousePosEvent(AMotionEvent_getX(aev, event_pointer_index), AMotionEvent_getY(aev, event_pointer_index));
						}
						else if (event_action == AMOTION_EVENT_ACTION_UP) {
							io.AddMousePosEvent(AMotionEvent_getX(aev, event_pointer_index), AMotionEvent_getY(aev, event_pointer_index));
							if (!ui_context_want_active_vk || vk_context_want_active) {
								io.AddMousePosEvent(-1, -1);
							}
						}
					}
				}
#endif
			}

			void nativeWindowEventListenerBegin(tp::glw::PlatformContext* ctx) {
				vk_context_want_active = vk_context->IO.WantCaptureMouse || vk_context->IO.WantSetMousePos;

				ui_context_want_active_vk = ui_context->IO.WantTextInput;
				ui_context_want_active = ui_context->IO.WantCaptureKeyboard || ui_context->IO.WantCaptureMouse || ui_context->IO.WantSetMousePos;
			}

			void nativeWindowEventListener(tp::glw::PlatformContext* ctx) {

				// proc vk
				if ((ui_context_want_active_vk || vk_context_want_active) && vk_first_drawn) {
					ImGui::SetCurrentContext(vk_context);
					nativeWindowEventListenerPlatform(ctx);

					while (vk_context->InputEventsQueue.Size) {
						ImGui::ApplyStyle(dpmm, font_size_mm, ui_scale);
						ImGui::NewFrame();
						VirtualKeyboard(dpmm, font_size_mm, ui_scale);
						ImGui::Render();

						vk_context_want_active |= vk_context->IO.WantCaptureMouse || vk_context->IO.WantSetMousePos;
					}

					vk_context_want_active |= vk_context->IO.WantCaptureMouse || vk_context->IO.WantSetMousePos;

					ImGui::SetCurrentContext(NULL);
				}

				// proc ui
				if (!vk_context_want_active && ui_first_drawn) {
					ImGui::SetCurrentContext(ui_context);
					nativeWindowEventListenerPlatform(ctx);

					while (ui_context->InputEventsQueue.Size) {
						ImGui::ApplyStyle(dpmm, font_size_mm, ui_scale);
						ImGui::NewFrame();
						cb(cd);
						ImGui::Render();

						ui_context_want_active_vk |= ui_context->IO.WantTextInput;
						ui_context_want_active |= ui_context->IO.WantCaptureKeyboard || ui_context->IO.WantCaptureMouse || ui_context->IO.WantSetMousePos;
					}

					ImGui::SetCurrentContext(NULL);
				}
			}

			void nativeWindowEventListenerEnd(tp::glw::PlatformContext* ctx) {}

			static void nativeWindowEventListenerWrapBegin(tp::glw::PlatformContext* ctx, void* cd) { ((DebugUiInternalContext*)cd)->nativeWindowEventListenerBegin(ctx); }
			static void nativeWindowEventListenerWrap(tp::glw::PlatformContext* ctx, void* cd) { ((DebugUiInternalContext*)cd)->nativeWindowEventListener(ctx); }
			static void nativeWindowEventListenerWrapEnd(tp::glw::PlatformContext* ctx, void* cd) { ((DebugUiInternalContext*)cd)->nativeWindowEventListenerEnd(ctx); }

			void VirtualKeyboard(tp::halnf dpmm, tp::halnf font_size_mm, tp::halnf ui_scale) {
				auto& config = mVKState.mConfigs[mVKState.mActiveConfigIdx];

				ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode);
				
				ImGuiDockNode* node = ImGui::DockContextFindNodeByID(ImGui::GetCurrentContext(), dockspace_id);

				if (!node->IsSplitNode()) {
					node->MergedFlags |= ImGuiDockNodeFlags_HiddenTabBar;
					ImGuiID dock_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.33f, nullptr, &dockspace_id);
					ImGui::DockBuilderDockWindow("VK", dock_id);
				}


				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 50 * dpmm, 30 * dpmm });
				ImGui::Begin("VK");
				ImGui::PopStyleVar(3);

				auto pos = ImGui::GetCursorPos();

				auto width = ImGui::GetWindowWidth();
				auto height = ImGui::GetWindowHeight() - pos.y;
				//auto scale = dpmm * ui_scale;
				auto padding = dpmm * 2 * ui_scale;
				auto button_sizey = (tp::halnf)(height - (config.nrows + 1) * padding) / config.nrows;

				tp::vec2f crs = { pos.x + padding, pos.y + padding };

				for (auto row = 0; row < config.nrows; row++) {
					tp::halnf butt_scalex = (width - (config.rows_sizes[row] + 1) * padding) / config.rows_widths[row];

					crs.x = pos.x + padding;

					for (auto butt = 0; butt < config.rows_sizes[row]; butt++) {
						auto& button = config.rows[row][butt];

						ImGui::SetCursorPos({ crs.x, crs.y });

						char name[2] = { mVKState.mShifted ? button.shift_val : button.val, 0 };

						ImGui::PushID(&button);
						if (!ImGui::Button(button.display_name ? button.display_name:  name, { butt_scalex * button.width, button_sizey })) {
							crs.x += butt_scalex * button.width + padding;
							ImGui::PopID();
							continue;
						}

						ImGui::SetCurrentContext(ui_context);
						button.action(&button, &mVKState);
						ImGui::SetCurrentContext(vk_context);

						ImGui::PopID();
					}

					crs.y += button_sizey + padding;
				}

				ImGui::End();
			}
		};
	}
}

tp::glw::DebugUI::DebugUI(Window& window, DebugUiDrawCallBack* acb, void* acd) {
	mDPMM = window.mDevice.mDPMM;

	// map device size to font and ui size
	{
		auto device_max = 600.f;
		auto device_min = 50.f;
		auto device = tp::clamp(window.mDevice.Size.x / mDPMM, device_min, device_max);
		auto device_factor = (device - device_min) / (device_max - device_min);

		auto font_min = 2.f;
		auto font_max = 3.5f;
		mFontSizeMM = (font_max - font_min) * device_factor + font_min;

		auto ui_max = 1.f;
		auto ui_min = 0.45f;
		mUIScale = (ui_max - ui_min) * device_factor + ui_min;
	}

	mCtx = new DebugUiInternalContext(window, acb, acd);
}

void tp::glw::DebugUI::drawDebugUI(tp::halnf dpmm) {
	mDPMM = dpmm;
	mCtx->drawDebugUI(mDPMM, mFontSizeMM, mUIScale);
}

tp::glw::DebugUI::~DebugUI() {
	delete mCtx;
}

// ------------------------------------------------------ // 

static const int sWindowFrameFlags = (ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration);

bool ImGui::SubMenuBegin(const char* desc, int level) {
	if (level == 1) {
		ImGui::Separator();
	}
	if (ImGui::TreeNode(desc)) {
		GImGui->Style.IndentSpacing = 6;
		return true;
	}
	return false;
}

void ImGui::SubMenuEnd(int level) {
	ImGui::TreePop();
}

void ImGui::ToolTip(const char* desc) {
	ImGui::SameLine();
	ImGui::TextDisabled("*");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

ImGui::PopupData ImGui::HoverPopupBeginButton(const char* id, tp::vec2f butsize, tp::vec2f popupsize) {
	ImVec2 curs = ImGui::GetCursorScreenPos();
	tp::vec2f popup_pos = tp::vec2f(curs.x + butsize.x / 2.f - popupsize.x / 2.f, (curs.y + butsize.y + 7));
	ImGui::Button(id, ImVec2(butsize.x, butsize.y)); ImGui::SameLine();
	return HoverPopupBegin(id, popupsize, popup_pos);
}

ImGui::PopupData ImGui::HoverPopupBegin(const char* str_id, tp::vec2f size, tp::vec2f pos_p, ImGuiPopupFlags popup_flags) {
	ImGui::PopupData out;
	out.ishovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

	if (out.ishovered) {
		ImVec2 pos;

		if (pos_p == tp::vec2f(-1)) {
			pos = GImGui->CurrentWindow->DC.CursorPos;
		}
		else {
			pos.x = pos_p.x;
			pos.y = pos_p.y;
		}

		ImGui::SetNextWindowPos(pos);
		out.p1 = { pos.x, pos.y };

		ImGui::OpenPopup(str_id);

		out.p2 = out.p1;
		out.p2.x += ImGui::GetWindowWidth();
	}

	if (BeginPopup(str_id, ImGuiWindowFlags_NoMove)) {
		out.opened = true;

		auto pos = GetWindowPos();
		out.p1 = { pos.x, pos.y };
		out.p2 = out.p1;
		out.p2.x += ImGui::GetWindowWidth();

	}
	return out;
}

void ImGui::HoverPopupEnd(ImGui::PopupData& in) {

	if (!in.opened) {
		return;
	}

	in.ishovered |= IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_ChildWindows);

	tp::vec2f mousepos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
	tp::halnf tollerance = 10;
	tp::rectf tollerace_rect = tp::rectf(tp::vec2f(in.p1.x, in.p1.y - tollerance), tp::vec2f(in.p2.x - in.p1.x, tollerance * 2.f));
	bool is_tollerance = tollerace_rect.inside(mousepos);

	if (!(in.ishovered || is_tollerance)) {
		CloseCurrentPopup();
	}

	EndPopup();
}

#define VAL(val) (val * dpmm / 3 * ui_scale)
#define VEC(x, y) ImVec2(x * dpmm / 3, y * dpmm / 3 * ui_scale)

void ImGui::ApplyStyle(tp::halnf dpmm, float font_size_mm, float ui_scale) {

	ImGuiStyle* style = &ImGui::GetStyle();
	auto& io = ImGui::GetIO();
	bool first_init = io.Fonts->Fonts.Size == 0;
	auto font_path = getFontPath();

	// supports PDMM from 0.3 to 10
	const float min_dpmm = 0.3f;
	const float max_dpmm = 20.f;
	dpmm = tp::clamp(dpmm, min_dpmm, max_dpmm);

	// calc font
	const float font_size_mm_max = 10;
	const float font_size_pixels_min = 9;
	float font_size_mm_min = font_size_pixels_min / dpmm;
	const int font_quality_steps = 3;
	font_size_mm = tp::clamp(font_size_mm, font_size_mm_min, font_size_mm_max);

	float font_sizes[font_quality_steps];
	for (auto i = 0; i < font_quality_steps; i++) {
		auto mm = font_size_mm_min + (font_size_mm_max - font_size_mm_min) * tp::pow(((tp::halnf) (i + 1) / font_quality_steps), 3);
		font_sizes[i] = dpmm * mm;
	}

	if (first_init) {
		for (auto i = 0; i < font_quality_steps; i++) {
			io.Fonts->AddFontFromFileTTF(font_path, font_sizes[i] * 1);
		}
	}

	// select fonts
	auto const pixels_required = dpmm * font_size_mm;
	auto idx = -1;
	for (auto i = 0; i < font_quality_steps; i++) {
		idx = i;
		if (pixels_required <= font_sizes[i]) {
			break;
		}
	}
	DBG_BREAK(idx == -1);
	io.FontDefault = io.Fonts->Fonts[idx];
	io.FontGlobalScale = font_size_mm / (font_size_mm_min + ((font_size_mm_max - font_size_mm_min) / font_quality_steps) * (idx + 1));
	io.FontGlobalScale = pixels_required / font_sizes[idx];

	auto rounding = VAL(5);
	auto pudding = VEC(7, 7);

	style->WindowPadding = pudding;
	style->WindowRounding = rounding;
	//style->WindowMinSize = VEC(11, 11);
	style->ChildRounding = rounding;
	style->PopupRounding = rounding;
	style->FramePadding = pudding;
	style->FrameRounding = rounding;
	style->ItemSpacing = VEC(4, 11);
	style->ItemInnerSpacing = VEC(9, 4);
	style->CellPadding = pudding;
	style->TouchExtraPadding = pudding;
	style->IndentSpacing = VAL(25);
	style->ColumnsMinSpacing = VAL(5);
	style->ScrollbarSize = VAL(16);
	style->ScrollbarRounding = rounding;
	style->GrabMinSize = VAL(2);
	style->GrabRounding = rounding;
	style->LogSliderDeadzone = VAL(2);
	style->TabRounding = rounding;
	style->TabMinWidthForCloseButton = VAL(5);
	style->DisplayWindowPadding = pudding;
	style->DisplaySafeAreaPadding = pudding;
	style->MouseCursorScale = VAL(5);

	style->FrameBorderSize = VAL(0);
	style->WindowBorderSize = VAL(1.5f);
	style->ChildBorderSize = VAL(2);

	style->WindowTitleAlign = VEC(0.5f, 0.6f);
	style->WindowMenuButtonPosition = ImGuiDir_Right;

	if (!first_init)
		return;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.27f, 0.27f, 0.33f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.43f, 0.43f, 0.53f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.09f, 0.09f, 0.10f, 0.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15f, 0.15f, 0.19f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.21f, 0.20f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.29f, 0.35f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.48f, 0.50f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.53f, 0.57f, 0.64f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.69f, 0.74f, 0.83f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.33f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.43f, 0.43f, 0.53f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.27f, 0.33f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.33f, 0.40f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.37f, 0.37f, 0.40f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.33f, 0.35f, 0.38f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.37f, 0.39f, 0.42f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.16f, 0.17f, 0.19f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.33f, 0.35f, 0.38f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.28f, 0.34f, 0.42f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.23f, 0.23f, 0.28f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.28f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.43f, 0.45f, 0.48f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.81f, 0.81f, 0.81f, 0.18f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.33f, 0.35f, 0.38f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.49f, 0.51f, 0.54f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.13f, 0.13f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.23f, 0.20f, 0.20f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.38f, 0.22f, 0.22f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.64f, 0.75f, 0.83f, 0.18f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.39f, 0.39f, 0.47f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.27f, 0.33f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.29f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.65f, 0.65f, 0.65f, 0.70f);

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
