
#pragma once

#include "env.h"

#ifdef ENV_OS_ANDROID
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#else
#include "GL/glew.h"
#endif 

#ifdef ENV_OS_ANDROID
#define GLW_CONTEXT_DEPTH_BITS 16
#define GLW_CONTEXT_DEPTH_COMPONENT GL_DEPTH_COMPONENT16
#else
#define GLW_CONTEXT_DEPTH_BITS 32
#define GLW_CONTEXT_DEPTH_COMPONENT GL_DEPTH_COMPONENT32
#endif 