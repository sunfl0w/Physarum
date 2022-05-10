#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <string>

#include "bitmapFont.hpp"
#include "shader.hpp"

namespace Tracer::Rendering::Text {
    class BitmapTextRenderer {
    private:
        BitmapFont bitmapFont;
        SDL_Window* window;
        Shader textShader;
        unsigned int VAO;
        unsigned int vertexVBO;

    public:
        BitmapTextRenderer(BitmapFont bitmapFont, SDL_Window* window);

        void DrawText(std::string text, glm::vec2 position, glm::vec2 scale) const;
    };
}  // namespace Tracer::Rendering::Text