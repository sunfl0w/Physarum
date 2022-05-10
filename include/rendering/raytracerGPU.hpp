#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "raytracer.hpp"
#include "shader.hpp"

namespace Tracer::Rendering {
    //Structs for arranging data that can be sent to the gpu
    struct Material {
        glm::vec4 color;
        glm::vec4 modifiers;  //First value is reflectivness, second one is transparency
    };

    struct Sphere {
        glm::vec4 position;
        glm::vec4 radius;
        Material material;
    };

    struct MeshObject {
        glm::vec4 vertexData[1024];
        glm::mat4 modelMatrix;
        glm::vec4 numTris;
        Material material;
    };

    struct PointLight {
        glm::vec4 position;
        glm::vec4 color;//Last vlaue is intensity
    };

    struct Camera {
        glm::mat4 modelMatrix;
    };

    struct SceneData {
        MeshObject meshObjects[8];
        Sphere spheres[4];
        PointLight pointLights[4];
        Camera camera;
        int numMeshObjects;
        int numSpheres;
        int numPointLights;
        int screenWidth;
        int screenHeight;
    };

    class RaytracerGPU : public Raytracer {
        Shader computeShader;
        Shader textureShader;
        unsigned int VAO;
        unsigned int texture;
        unsigned int shaderDataBuffer;
    public:
        RaytracerGPU(SDL_Window* window);

        virtual void RenderSceneToWindow(Scene& scene) const;

    private:
        SceneData ConvertSceneToStruct(Scene& scene) const;
    };
}  // namespace Tracer::Rendering