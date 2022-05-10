#include "bitmapTextRenderer.hpp"

namespace Tracer::Rendering::Text {
    BitmapTextRenderer::BitmapTextRenderer(BitmapFont bitmapFont, SDL_Window* window) : bitmapFont(bitmapFont) {
        textShader = Shader("resources/shaders/texture.vs", GL_VERTEX_SHADER, "resources/shaders/texture.fs", GL_FRAGMENT_SHADER);
        this->window = window;

        float vertexData[] = {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f};

        unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3};

        unsigned int EBO;

        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &vertexVBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void BitmapTextRenderer::DrawText(std::string text, glm::vec2 position, glm::vec2 scale) const {
        int windowWidth = 0;
        int windowHeight = 0;

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        //UI begins in the upper left corner
        //Flip y scale so text coords start in the upper left corner of the text
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position.x, windowHeight - position.y - scale.y, 0.0f));
        model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));

        glm::mat4 textProjection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);

        for (char character : text) {
            int charactersPerRow = bitmapFont.GetWidth() / bitmapFont.GetCharacterSize();
            int charactersPerColumn = bitmapFont.GetHeight() / bitmapFont.GetCharacterSize();

            float texturePosLeft = ((character - 32) % (charactersPerRow)) * 1.0f / charactersPerRow;
            float texturePosRight = texturePosLeft + 1.0f / charactersPerRow;
            float texturePosUp = 1.0f - (std::floor((character - 32) / charactersPerRow) * 1.0f / charactersPerColumn);
            float texturePosDown = texturePosUp - 1.0f / charactersPerColumn;

            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);

            float vertexData[] = {
                1.0f, 1.0f, texturePosRight, texturePosUp,
                1.0f, 0.0f, texturePosRight, texturePosDown,
                0.0f, 0.0f, texturePosLeft, texturePosDown,
                0.0f, 1.0f, texturePosLeft, texturePosUp};

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            textShader.Activate();
            textShader.SetMat4("pv", textProjection);
            textShader.SetMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            bitmapFont.Activate();

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }
}  // namespace Tracer::Rendering::Text