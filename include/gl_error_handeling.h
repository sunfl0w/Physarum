#pragma once

#include "deps/glad/glad.h"

#define glCall(call)                                                                                                                                                                                                       \
    call;                                                                                                                                                                                                                  \
    gl_check(__FILE__, __LINE__, #call)

GLenum gl_check(const char* file, int line, const char* call);

void APIENTRY gl_debug_msg_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data);