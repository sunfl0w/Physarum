#include "raytracerGPU.hpp"

namespace Tracer::Rendering {
    RaytracerGPU::RaytracerGPU(SDL_Window* window) : Raytracer(window) {
        computeShader = Shader("resources/shaders/raytracingSimple.comp", GL_COMPUTE_SHADER);
        textureShader = Shader("resources/shaders/raytracedImage.vs", GL_VERTEX_SHADER, "resources/shaders/raytracedImage.fs", GL_FRAGMENT_SHADER);

        //Populate buffer objects for later rendering of the texture
        float vertices[] = {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};

        unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3};

        unsigned int VBO, EBO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //Creating a texture, make it an image and bind it to an image unit
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Create buffer for the compute shader
        glGenBuffers(1, &shaderDataBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderDataBuffer);
        SceneData emptySceneData;
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(emptySceneData), &emptySceneData, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, shaderDataBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void RaytracerGPU::RenderSceneToWindow(Scene& scene) const {
        SceneData sceneData = ConvertSceneToStruct(scene);

        computeShader.Activate();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderDataBuffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(sceneData), &sceneData);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glDispatchCompute(screenWidth / 8.0f, screenHeight / 8.0f, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        textureShader.Activate();
        textureShader.SetInt("tex", 0);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    SceneData RaytracerGPU::ConvertSceneToStruct(Scene& scene) const {
        SceneData sceneData;
        sceneData.screenWidth = screenWidth;
        sceneData.screenHeight = screenHeight;

        //MeshObjects
        int meshObjectIndex = 0;
        for (std::unique_ptr<Objects::MeshObject>& meshObject : scene.GetMeshObjects()) {
            MeshObject meshObjectData;
            int vertexIndex = 0;
            int triangleCount = 0;
            for (Math::Tris triangle : meshObject->GetMesh().GetData()) {
                meshObjectData.vertexData[vertexIndex] = glm::vec4(triangle.vert0, 1.0f);
                meshObjectData.vertexData[vertexIndex + 1] = glm::vec4(triangle.vert1, 1.0f);
                meshObjectData.vertexData[vertexIndex + 2] = glm::vec4(triangle.vert2, 1.0f);
                vertexIndex += 3;
                triangleCount++;
            }
            meshObjectData.modelMatrix = meshObject->GetTransform().GetTransformMatrix();
            meshObjectData.numTris = glm::vec4(triangleCount, 0, 0, 0);
            meshObjectData.material.color = glm::vec4(meshObject->GetMaterial().GetColor(), 0);
            meshObjectData.material.modifiers.x = meshObject->GetMaterial().GetReflectivity();
            meshObjectData.material.modifiers.y = meshObject->GetMaterial().GetTransparency();
            sceneData.meshObjects[meshObjectIndex] = meshObjectData;
            meshObjectIndex++;
        }
        sceneData.numMeshObjects = meshObjectIndex;

        //Spheres
        int sphereIndex = 0;
        for (std::unique_ptr<Objects::Sphere>& sphere : scene.GetSpheres()) {
            Sphere sphereData;
            sphereData.radius[0] = sphere->GetRadius();
            sphereData.position = glm::vec4(sphere->GetTransform().GetPosition(), 0);
            sphereData.material.color = glm::vec4(sphere->GetMaterial().GetColor(), 0);
            sphereData.material.modifiers.x = sphere->GetMaterial().GetReflectivity();
            sphereData.material.modifiers.y = sphere->GetMaterial().GetTransparency();
            sceneData.spheres[sphereIndex] = sphereData;
            sphereIndex++;
        }
        sceneData.numSpheres = sphereIndex;

        //Lights
        int lightIndex = 0;
        for (std::unique_ptr<Objects::PointLight>& pointLight : scene.GetPointLights()) {
            PointLight lightData;
            lightData.position = glm::vec4(pointLight->GetTransform().GetPosition(), 0);
            lightData.color = glm::vec4(pointLight->GetColor().r, pointLight->GetColor().g, pointLight->GetColor().b, pointLight->GetIntensity());
            sceneData.pointLights[lightIndex] = lightData;
            lightIndex++;
        }
        sceneData.numPointLights = lightIndex;

        //Camera
        Camera cameraData;
        cameraData.modelMatrix = scene.GetCamera().GetTransform().GetTransformMatrix();
        sceneData.camera = cameraData;

        return sceneData;
    }
}  // namespace Tracer::Rendering