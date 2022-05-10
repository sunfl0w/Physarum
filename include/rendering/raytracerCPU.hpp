#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "raytracer.hpp"
#include "shader.hpp"
#include "intersectionData.hpp"

namespace Tracer::Rendering {
    class RaytracerCPU : public Raytracer {
        Shader textureShader;
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        unsigned int texture;

    public:
        RaytracerCPU(SDL_Window* window);

        virtual void RenderSceneToWindow(Scene& scene) const;

    private:
        std::vector<unsigned char> RenderSceneToBuffer(Scene& scene) const;

        glm::vec3 Raytrace(Scene& scene, glm::vec3& origin, glm::vec3& dir, int depth) const;

        Tracer::Math::IntersectionData RaycastObjects(std::vector<std::unique_ptr<Objects::RenderableObject>>& renderableObjects, glm::vec3& origin, glm::vec3& dir) const;
    };
}  // namespace Tracer::Rendering