#include "raytracer.hpp"

namespace Tracer::Rendering {
    Raytracer::Raytracer(SDL_Window* window) {
        this->window = window;
        SDL_GetWindowSize(window, &screenWidth, &screenHeight);
    }

    Raytracer::~Raytracer() {}
}  // namespace Tracer::Rendering