#include "gl_error_handeling.h"

#include <stdio.h>

GLenum gl_check(const char* file, int line, const char* call) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        char* error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error = "UNKNOWN_ERROR";
                break;
        }
        printf("%s | %s | %i | %s\n", error, file, line, call);
    }
    return errorCode;
}

void APIENTRY gl_debug_msg_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data) {
    char* source_type;
    char* error_type;
    char* severity_type;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        source_type = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_type = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_type = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_type = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        source_type = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        source_type = "OTHER";
        break;

    default:
        source_type = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        error_type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        error_type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        error_type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        error_type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        error_type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        error_type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        error_type = "MARKER";
        break;

    default:
        error_type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severity_type = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        severity_type = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        severity_type = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity_type = "NOTIFICATION";
        break;

    default:
        severity_type = "UNKNOWN";
        break;
    }

    printf("Error/Notification\nID %i\nTYPE %s\nSEVERITY %s\nSOURCE %s\nMESSAGE %s\n", id, error_type, severity_type, source_type, message);
}