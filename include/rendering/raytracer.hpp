#pragma once
#include <SDL2/SDL.h>

#include "scene.hpp"

namespace Tracer::Rendering {
    class Raytracer {
    protected:
        SDL_Window* window;
        int screenWidth;
        int screenHeight;

    public:
        Raytracer(SDL_Window* window);

        virtual ~Raytracer();

        virtual void RenderSceneToWindow(Scene& scene) const = 0;
    };
}  // namespace Tracer::Rendering