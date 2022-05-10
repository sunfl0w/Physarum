#pragma once

#include <glad/glad.h>

#include <iostream>
#include <string>

namespace Tarcer::Rendering::GLDebugging {
    void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data) {
        std::string sourceType;
        std::string errorType;
        std::string severityType;

        switch (source) {
            case GL_DEBUG_SOURCE_API:
                sourceType = "API";
                break;

            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                sourceType = "WINDOW SYSTEM";
                break;

            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                sourceType = "SHADER COMPILER";
                break;

            case GL_DEBUG_SOURCE_THIRD_PARTY:
                sourceType = "THIRD PARTY";
                break;

            case GL_DEBUG_SOURCE_APPLICATION:
                sourceType = "APPLICATION";
                break;

            case GL_DEBUG_SOURCE_OTHER:
                sourceType = "UNKNOWN";
                break;

            default:
                sourceType = "UNKNOWN";
                break;
        }

        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                errorType = "ERROR";
                break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                errorType = "DEPRECATED BEHAVIOR";
                break;

            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                errorType = "UDEFINED BEHAVIOR";
                break;

            case GL_DEBUG_TYPE_PORTABILITY:
                errorType = "PORTABILITY";
                break;

            case GL_DEBUG_TYPE_PERFORMANCE:
                errorType = "PERFORMANCE";
                break;

            case GL_DEBUG_TYPE_OTHER:
                errorType = "OTHER";
                break;

            case GL_DEBUG_TYPE_MARKER:
                errorType = "MARKER";
                break;

            default:
                errorType = "UNKNOWN";
                break;
        }

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                severityType = "HIGH";
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                severityType = "MEDIUM";
                break;

            case GL_DEBUG_SEVERITY_LOW:
                severityType = "LOW";
                break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
                severityType = "NOTIFICATION";
                break;

            default:
                severityType = "UNKNOWN";
                break;
        }
        std::cout << "Error/Notification"
                  << "\n";
        std::cout << "ID: " << id << "\n";
        std::cout << "Type: " << errorType << "\n";
        std::cout << "Severity: " << severityType << "\n";
        std::cout << "Origin: " << source << "\n";
        std::cout << "Message: " << message << "\n";
        std::cout << "------------------"
                  << "\n";
    }
}  // namespace Tarcer::Rendering