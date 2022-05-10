#include "glad.h"

#include <SDL2/SDL.h>

// GL_DEBUG

#define glCall(call)                                                                                                                                                                                                       \
    call;                                                                                                                                                                                                                  \
    glCheckError(__FILE__, __LINE__, #call)

GLenum glCheckError(const char* file, int line, const char* call) {
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
        }
        printf("%s | %s | %i | %s\n", error, file, line, call);
    }
    return errorCode;
}

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data) {
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

int main(int argc, char* argv[]) {
    uint32_t screen_width = 600;
    uint32_t screen_height = 400;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow("Physarum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_OPENGL);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(0);

    if (!gladLoadGL()) {
        printf("Unable to load OpenGL\n");
        return -1;
    }

#ifdef ENABLE_OPENGL_DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugMessageCallback, 0);
#endif
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    int32_t running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }
    return 0;
}