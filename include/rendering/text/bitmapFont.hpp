#pragma once

#include <glad.h>
#include <stb_image.h>

#include <iostream>
#include <string>

namespace Tracer::Rendering::Text {
    class BitmapFont {
    private:
        unsigned int bitmapID;
        unsigned int characterSize;
        int width;
        int height;

    public:
        BitmapFont(std::string bitmapPath, unsigned int characterSize);

        unsigned int GetCharacterSize() const;

        int GetWidth() const;

        int GetHeight() const;

        void Activate() const;

        unsigned int GetID() const;
    };
}  // namespace Tracer::Rendering::Text