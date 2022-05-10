#include "bitmapFont.hpp"

namespace Tracer::Rendering::Text {
    BitmapFont::BitmapFont(std::string bitmapPath, unsigned int characterSize) {
        this->characterSize = characterSize;

        glGenTextures(1, &bitmapID);
        glBindTexture(GL_TEXTURE_2D, bitmapID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_set_flip_vertically_on_load(true);

        int channelCount;
        unsigned char *data = stbi_load(bitmapPath.c_str(), &width, &height, &channelCount, 0);
        if (data) {
            if (channelCount == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else if (channelCount == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        } else {
            std::cout << "Failed to load bitmap\n";
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    unsigned int BitmapFont::GetCharacterSize() const {
        return characterSize;
    }

    int BitmapFont::GetWidth() const {
        return width;
    }

    int BitmapFont::GetHeight() const {
        return height;
    }

    void BitmapFont::Activate() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bitmapID);
    }

    unsigned int BitmapFont::GetID() const {
        return bitmapID;
    }
}  // namespace Tracer::Rendering::Text
