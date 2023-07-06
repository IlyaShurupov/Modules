 
 // Copyright (C) 2010 The Android Open Source Project
 // Licensed under the Apache License, Version 2.0 (the "License");
 // you may not use this file except in compliance with the License.
 // You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 // Unless required by applicable law or agreed to in writing, software
 // distributed under the License is distributed on an "AS IS" BASIS,
 // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 // See the License for the specific language governing permissions and
 // limitations under the License.

// own headers
#include "window.h"

#include "GraphicsLibApi.h"

// STL
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

// jni
//#include <initializer_list>
//#include <memory>
//#include <cstdlib>
//#include <cstring>
#include <jni.h>
//#include <cerrno>
//#include <cassert>

// opengl
#include <EGL/egl.h>
#include <GLES3/gl32.h>
//#include "GL/glew.h"

// glue
#include <android/sensor.h>
#include <android/log.h>
#include  <android/input.h>
#include <../../../android/native_app_glue/android_native_app_glue.h>

// AcquireASensorManagerInstance
#include <dlfcn.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

// application side entry point
int main();

// first function to be called in "main thread"
void* main_wrap(void* p);

 // AcquireASensorManagerInstance(void)
 // Workaround ASensorManager_getInstance() deprecation false alarm
 // for Android-N and before, when compiling with NDK-r15
ASensorManager* AcquireASensorManagerInstance(android_app* app) {

  if (!app)
    return nullptr;

  typedef ASensorManager* (*PF_GETINSTANCEFORPACKAGE)(const char* name);
  void* androidHandle = dlopen("libandroid.so", RTLD_NOW);
  auto getInstanceForPackageFunc = (PF_GETINSTANCEFORPACKAGE)
    dlsym(androidHandle, "ASensorManager_getInstanceForPackage");
  if (getInstanceForPackageFunc) {
    JNIEnv* env = nullptr;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    jclass android_content_Context = env->GetObjectClass(app->activity->clazz);
    jmethodID midGetPackageName = env->GetMethodID(android_content_Context,
      "getPackageName",
      "()Ljava/lang/String;");
    auto packageName = (jstring)env->CallObjectMethod(app->activity->clazz,
      midGetPackageName);

    const char* nativePackageName = env->GetStringUTFChars(packageName, nullptr);
    ASensorManager* mgr = getInstanceForPackageFunc(nativePackageName);
    env->ReleaseStringUTFChars(packageName, nativePackageName);
    app->activity->vm->DetachCurrentThread();
    if (mgr) {
      dlclose(androidHandle);
      return mgr;
    }
  }

  typedef ASensorManager* (*PF_GETINSTANCE)();
  auto getInstanceFunc = (PF_GETINSTANCE)
    dlsym(androidHandle, "ASensorManager_getInstance");
  // by all means at this point, ASensorManager_getInstance should be available
  assert(getInstanceFunc);
  dlclose(androidHandle);

  return getInstanceFunc();
}

void GL_APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
  DBG_BREAK(true);
  LOGW("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

char const* gl_error_string(GLenum const err) {
  switch (err) {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default: return "unknown error";
  }
}

void glerr(GLenum type) {
  DBG_BREAK(true);
  const GLchar* message = gl_error_string(type);
  LOGW("GL CALLBACK: %s type = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, message);
}

void glerr(GLenum type);
#define AssertGL(x) { x; GLenum __gle = glGetError(); if (__gle != GL_NO_ERROR) glerr(__gle); }

// --------------------------------- Data --------------------------------------- //

struct MutexEvent {
  pthread_mutex_t mutex;
  tp::glw::Window* active_windw;
  bool parent_thread_has_events = false;
  bool child_whaiting = false;
} mutex_event;

struct ANativeActivityCtx* android_ctx = NULL;

tp::glw::Window::Device tp::glw::Window::mDevice;
extern const char* g_android_internal_data_path;

static pthread_t worker_thread;
static pthread_mutex_t worker_finished_mutex;
static pthread_cond_t worker_finished_cond;
static bool worker_finished = false;

namespace tp {
  namespace glw {
    struct PlatformContext {

      int col = 255;

      EGLint w, h, format;
      EGLint numConfigs;
      EGLConfig config = nullptr;
      EGLSurface surface;
      EGLContext context;
      EGLDisplay display = nullptr;

      AInputEvent* chache_event = NULL;

      EGLint majorVersion, minorVersion;

      // Initialize an EGL context for the current display.
      PlatformContext(ANativeWindow* window) {

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (EGL_NO_DISPLAY == display) {
          return;
        }

        
        if (eglInitialize(display, &majorVersion, &minorVersion) != EGL_TRUE) {
          return;
        }


        // Here specify the attributes of the desired configuration.
        // Below, we select an EGLConfig with at least 8 bits per color component compatible with on-screen windows
        const EGLint attribs[] = {
          EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, // request OpenGL ES 3.0
          EGL_BLUE_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_RED_SIZE, 8,
          EGL_DEPTH_SIZE, GLW_CONTEXT_DEPTH_BITS,
          EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
          EGL_NONE
        };

        // Here, the application chooses the configuration it desires.
        // find the best match if possible, otherwise use the very first one 
        {
          if (eglChooseConfig(display, attribs, nullptr, 0, &numConfigs) != EGL_TRUE) {
            return;
          }

          std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
          assert(supportedConfigs);

          if (eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs) != EGL_TRUE) {
            return;
          }

          assert(numConfigs);
          auto i = 0;
          for (; i < numConfigs; i++) {
            auto& cfg = supportedConfigs[i];
            EGLint r, g, b, d;
            if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
              eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
              eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
              eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
              r == 8 && g == 8 && b == 8 && d == GLW_CONTEXT_DEPTH_BITS) {

              config = supportedConfigs[i];
              break;
            }
          }

          if (i == numConfigs) {
            config = supportedConfigs[0];
          }

          if (config == nullptr) {
            LOGW("Unable to initialize EGLConfig");
            return;
          }
        }

        const EGLint egl_context_attributes[] = { 
          EGL_CONTEXT_CLIENT_VERSION, 3, 
          //EGL_CONTEXT_MAJOR_VERSION, 3,
          //EGL_CONTEXT_MINOR_VERSION, 0,
          EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifdef ENV_BUILD_DEBUG
          //EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
          EGL_NONE 
        };

        // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
        // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
        // As soon as we picked a EGLConfig, we can safely reconfigure the
        // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID
        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
        surface = eglCreateWindowSurface(display, config, window, nullptr);
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, egl_context_attributes);

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
          LOGW("Unable to eglMakeCurrent");
          return;
        }

        eglQuerySurface(display, surface, EGL_WIDTH, &w);
        eglQuerySurface(display, surface, EGL_HEIGHT, &h);

        // Check openGL on the system
        auto opengl_info = { GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS };
        for (auto name : opengl_info) {
          auto info = glGetString(name);
          LOGI("OpenGL Info: %s", info);
        }

        // Initialize GL state.
        //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
        AssertGL(glEnable(GL_CULL_FACE));
        //AssertGL(glShadeModel(GL_SMOOTH));
        AssertGL(glEnable(GL_DEPTH_TEST));
        AssertGL(glDepthFunc(GL_LESS));

        GLint major = 0;
        GLint minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        glGetIntegerv(GL_DEPTH, &minor);

        AssertGL(glEnable(GL_DEBUG_OUTPUT));
        AssertGL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        AssertGL(glDebugMessageCallback(MessageCallback, 0));

        tp::glw::Window::mDevice.Size = { w, h };
        tp::glw::Window::mDevice.FrameRate = 60;

        return;
      }

      void beginDraw() {
        if (this->display == nullptr) { return; }

        glViewport(0, 0, tp::glw::Window::mDevice.Size.x, tp::glw::Window::mDevice.Size.y);

        //glClearColor(col / 255.f, col / 255.f, col / 255.f, 1);
        //glClear(GL_COLOR_BUFFER_BIT);

        //if (col++ > 255) {
          //col = 0;
        //}
      }

      void endDraw() {
        eglSwapBuffers(display, surface);
      }

      // Tear down the EGL context currently associated with the display.
      ~PlatformContext() {
        if (this->display != EGL_NO_DISPLAY) {
          eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
          if (this->context != EGL_NO_CONTEXT) {
            eglDestroyContext(this->display, this->context);
          }
          if (this->surface != EGL_NO_SURFACE) {
            eglDestroySurface(this->display, this->surface);
          }
          eglTerminate(this->display);
        }

        display = EGL_NO_DISPLAY;
        context = EGL_NO_CONTEXT;
        surface = EGL_NO_SURFACE;
      }
    };
  };
};

// ---------------------------------- Parent Thread ------------------------------------ //
#ifndef PARENT_THREAD

// Shared state for our app.
struct ANativeActivityCtx {
  
  //  Our saved state data.
  struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
  };

  android_app* app;

  ASensorManager* sensorManager;
  const ASensor* accelerometerSensor;
  ASensorEventQueue* sensorEventQueue;

  int animating;
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  int32_t width;
  int32_t height;
  saved_state state;

  ANativeActivityCtx(android_app* state) {
    memset(this, 0, sizeof(*this));

    state->userData = this;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;

    app = state;

    // Prepare to monitor accelerometer
    sensorManager = AcquireASensorManagerInstance(state);
    accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, state->looper, LOOPER_ID_USER, 0, 0);

    // We are starting with a previous saved state; restore from it.
    if (state->savedState != nullptr) {
      this->state = *(saved_state*)state->savedState;
    }
  }

  void proc_events_util() {
    int ident;
    int events;
    struct android_poll_source* source;

    auto window = mutex_event.active_windw;

    if (window) {
      for (auto proc : window->mNativeEventListeners) {
        proc->val.exec_begin(window->mPlatformCtx, proc->val.cd);
      }
    }

    // we loop until all events are read
    while ((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0) {

      // Process this event.
      if (source != nullptr) {
        source->process(this->app, source);
      }

      // If a sensor has data, process it now.
      if (ident == LOOPER_ID_USER) {
        if (this->accelerometerSensor != nullptr) {
          ASensorEvent event;
          while (ASensorEventQueue_getEvents(this->sensorEventQueue, &event, 1) > 0) {
            LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
          }
        }
      }

      // Check if we are exiting.
      if (this->app->destroyRequested != 0) {
        DBG_BREAK(1);
      }
    }

    if (window) {
      for (auto proc : window->mNativeEventListeners) {
        proc->val.exec_end(window->mPlatformCtx, proc->val.cd);
      }
    }
  }

  void proc_events() {

    pthread_mutex_lock(&mutex_event.mutex);
    auto threaded = mutex_event.active_windw;
    pthread_mutex_unlock(&mutex_event.mutex);

    if (!threaded) {
      proc_events_util();
      return;
    }

    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;

    // check if events exist and if so prepare to proccess them
    if ((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0) {

      // signal child thread that we need to process those events
      pthread_mutex_lock(&mutex_event.mutex);
      mutex_event.parent_thread_has_events = true;
      pthread_mutex_unlock(&mutex_event.mutex);

      // whait until child thread pauses
      bool whait = true;

      while (whait) {
        pthread_mutex_lock(&mutex_event.mutex);
        whait = !mutex_event.child_whaiting;
        pthread_mutex_unlock(&mutex_event.mutex);
      }
    }
    else {
      return;
    }


    pthread_mutex_lock(&mutex_event.mutex);
    auto window = mutex_event.active_windw;
    pthread_mutex_unlock(&mutex_event.mutex);

    proc_events_util();

    // signal to child continue to do its work
    pthread_mutex_lock(&mutex_event.mutex);
    mutex_event.parent_thread_has_events = false;
    pthread_mutex_unlock(&mutex_event.mutex);
  }

  // Process the next input event.
  static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    auto* engine = (struct ANativeActivityCtx*)app->userData;
    auto window = mutex_event.active_windw;

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
      window->mEvents.mCursorPrev = window->mEvents.mCursor;

      window->mEvents.mCursor.x = AMotionEvent_getX(event, 0);
      window->mEvents.mCursor.y = AMotionEvent_getY(event, 0);
      
      window->mEvents.mRedraw = 2;

      int32_t event_action = AMotionEvent_getAction(event);
      int32_t event_pointer_index = (event_action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
      event_action &= AMOTION_EVENT_ACTION_MASK;

      if ((AMotionEvent_getToolType(event, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_FINGER)
        || (AMotionEvent_getToolType(event, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_UNKNOWN)) {

        if (event_action == AMOTION_EVENT_ACTION_DOWN) {
          window->mEvents.mKeysQueue.push(tp::KeyEvent{ tp::Keycode::MOUSE1, tp::KeyEvent::EventState::PRESSED });
        }
        else if (event_action == AMOTION_EVENT_ACTION_UP) {
          window->mEvents.mKeysQueue.push(tp::KeyEvent{ tp::Keycode::MOUSE1, tp::KeyEvent::EventState::RELEASED });
        }
      }
    }

    if (window) {
      window->mPlatformCtx->chache_event = event;
      for (auto proc : window->mNativeEventListeners) {
        proc->val.exec(window->mPlatformCtx, proc->val.cd);
      }
    }

    return 0;
  }

  // Process the next main command.
  static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    auto* engine = (struct ANativeActivityCtx*)app->userData;
    auto window = mutex_event.active_windw;

    switch (cmd) {

      // The system has asked us to save our current state.  Do so.
    case APP_CMD_SAVE_STATE: {
      engine->app->savedState = malloc(sizeof(ANativeActivityCtx::saved_state));
      *((ANativeActivityCtx::saved_state*)engine->app->savedState) = engine->state;
      engine->app->savedStateSize = sizeof(ANativeActivityCtx::saved_state);
    } break;

      // The window is being shown, get it ready.
    case APP_CMD_INIT_WINDOW: {
      if (engine->app->window != nullptr) {
        //engine->init_display();
        //engine->draw_frame();
      }
    } break;

      // The window is being hidden or closed, clean it up.
    case APP_CMD_TERM_WINDOW: {
      //engine->term_display();
    } break;

      // When our app gains focus, we start monitoring the accelerometer.
    case APP_CMD_GAINED_FOCUS: {
      if (engine->accelerometerSensor != nullptr) {
        ASensorEventQueue_enableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(engine->sensorEventQueue,
          engine->accelerometerSensor,
          (1000L / 60) * 1000);
      }
    } break;

      // When our app loses focus, we stop monitoring the accelerometer.
      // This is to avoid consuming battery while not being used.
    case APP_CMD_LOST_FOCUS: {
      if (engine->accelerometerSensor != nullptr) {
        ASensorEventQueue_disableSensor(engine->sensorEventQueue,
          engine->accelerometerSensor);
      }
      // Also stop animating.
      engine->animating = 0;
      //engine->draw_frame();
    } break;

    default: break;
    }
  }

};

#include <sys/stat.h>

void unsure_dir_exist(const char* path_to_file) {
  char* temp_path = strdup(path_to_file);

  for (char* p = temp_path + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      int status = mkdir(temp_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if (status != 0 && errno != EEXIST) {
        // handle error
        break;
      }
      *p = '/';
    }
  }

  free(temp_path);
}

// FIXME
void unpackAsset(AAssetManager* mgr, const char* filename, const std::string& path) {
  AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
  if (asset == NULL) {
    return;
  }
  off_t length = AAsset_getLength(asset);
  char* buffer = new char[length];
  AAsset_read(asset, buffer, length);

  unsure_dir_exist(path.c_str());
  FILE* out = fopen(path.c_str(), "w");
  if (out != NULL) {
    fwrite(buffer, length, 1, out);
    fclose(out);
  }
  delete[] buffer;
  AAsset_close(asset);
}

// FIXME
void unpackDirectory(AAssetManager* mgr, const char* dirname, const std::string& path) {

  AAssetDir* assetDir = AAssetManager_openDir(mgr, dirname);
  const char* filename = NULL;

  while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
    std::string assetPath = std::string(dirname) + "/" + filename;
    std::string internalPath = path + "/" + std::string(dirname) + "/" + filename;

    struct stat sb;
    stat(assetPath.c_str(), &sb);
    if (S_ISDIR(sb.st_mode)) {
      unpackDirectory(mgr, assetPath.c_str(), internalPath);
    }
    else {
      unpackAsset(mgr, assetPath.c_str(), internalPath);
    }
  }
  AAssetDir_close(assetDir);
}

extern "C" {

  // This is the main entry point of a native application that is using
  // android_native_app_glue.  It runs in its own thread, with its own
  // event loop for receiving input events and doing other things.
  void android_main(android_app* state) {

    tp::print_env_info();
    tp::alloc_init();

    ANativeActivityCtx engine(state);
    
    unpackDirectory(state->activity->assetManager, "", state->activity->internalDataPath);
    unpackDirectory(state->activity->assetManager, "rsc", state->activity->internalDataPath);
    unpackDirectory(state->activity->assetManager, "rsc/fonts", state->activity->internalDataPath);

    android_ctx = &engine;

    bool running = true;
    while (running) {
      
      engine.proc_events();

      // whait until our app fully initialized
      if (!engine.app->window) {
        continue;
      }

      // run once
      static bool main_thread_started = false;
      if (!main_thread_started) {
        
        // init types
        g_android_internal_data_path = state->activity->internalDataPath;

        // create main thread
        pthread_create(&worker_thread, NULL, main_wrap, NULL);
        pthread_mutex_init(&worker_finished_mutex, NULL);
        pthread_cond_init(&worker_finished_cond, NULL);

        main_thread_started = true;
        continue;
      } 

      // check for exit
      pthread_mutex_lock(&worker_finished_mutex);
      running = !worker_finished;
      pthread_mutex_unlock(&worker_finished_mutex);
    }

    pthread_join(worker_thread, NULL);

    tp::alloc_uninit();
  }
};
#endif


// -------------------------------  Child thread --------------------------------------- //
#ifndef CHILD_THREAD

//#include <android/configuration.h>
//#include <android/native_activity.h>

AInputEvent* androidPlatformContextGetEvent(tp::glw::PlatformContext* ctx) {
  return ctx->chache_event;
}

int32_t getDensityDpi(android_app* app) {
  AConfiguration* config = AConfiguration_new();
  AConfiguration_fromAssetManager(config, app->activity->assetManager);
  int32_t density = AConfiguration_getDensity(config);
  AConfiguration_delete(config);
  return density;
}

// statics
void tp::glw::Window::init() {}
void tp::glw::Window::deinit() {}

void tp::glw::Window::initialize() {
  mPlatformCtx = new PlatformContext(android_ctx->app->window);
  applyAppearance();


  AConfiguration* config = AConfiguration_new();
  AConfiguration_fromAssetManager(config, android_ctx->app->activity->assetManager);
  int32_t density = AConfiguration_getDensity(config);
  AConfiguration_delete(config);

  mDevice.mDPMM = density / 25.4f;
}

tp::glw::Window::Window() {
  initialize();

  pthread_mutex_lock(&mutex_event.mutex);
  mutex_event.active_windw = this;
  pthread_mutex_unlock(&mutex_event.mutex);
}

tp::glw::Window::Window(const Appearence& aAppearence) {
  mAppearence = aAppearence;
  initialize();
}

tp::glw::Window::~Window() {
  delete mPlatformCtx;
}

void tp::glw::Window::applyAppearance() {
  int width = ANativeWindow_getWidth(android_ctx->app->window);
  int height = ANativeWindow_getHeight(android_ctx->app->window);

  if (width != mAppearence.mSize.x || height != mAppearence.mSize.y) {
    mEvents.mRedraw = 2;
  }

  mAppearence.mPos = mCache.mAppearencePrev.mPos = 0;
  mAppearence.mSize.x = mCache.mAppearencePrev.mSize.x = tp::glw::Window::mDevice.Size.x = width;
  mAppearence.mSize.y = mCache.mAppearencePrev.mSize.y = tp::glw::Window::mDevice.Size.y = height;
}

void tp::glw::Window::pollEvents() {
  applyAppearance();

  // check if parent thread has some events to pass

  pthread_mutex_lock(&mutex_event.mutex);
  bool has_event = mutex_event.parent_thread_has_events;
  mutex_event.child_whaiting = has_event;
  pthread_mutex_unlock(&mutex_event.mutex);

  while (has_event) {
    pthread_mutex_lock(&mutex_event.mutex);
    has_event = mutex_event.parent_thread_has_events;
    pthread_mutex_unlock(&mutex_event.mutex);
  }

  pthread_mutex_lock(&mutex_event.mutex);
  mutex_event.child_whaiting = false;
  pthread_mutex_unlock(&mutex_event.mutex);
}

void tp::glw::Window::platformCallback() {}

void tp::glw::Window::beginDraw() {
  mPlatformCtx->beginDraw();
}

void tp::glw::Window::endDraw() {
  mPlatformCtx->endDraw();
  if (mEvents.mRedraw-- < 0) mEvents.mRedraw = 0;
}

void* tp::glw::Window::platform_window() const { return android_ctx->app->window; }

tp::alni tp::glw::Window::sizeAllocatedMem() { return 0; }

tp::alni tp::glw::Window::sizeUsedMem() { return 0; }


void* main_wrap(void* p) {

  // exec main
  main();

  // mark as doen after completion
  pthread_mutex_lock(&worker_finished_mutex);
  worker_finished = true;
  pthread_cond_signal(&worker_finished_cond);
  pthread_mutex_unlock(&worker_finished_mutex);

  // exit "main thread"
  pthread_exit(NULL);

  return NULL;
}

#endif