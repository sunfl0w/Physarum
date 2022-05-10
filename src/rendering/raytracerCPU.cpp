#include "raytracerCPU.hpp"

namespace Tracer::Rendering {
    RaytracerCPU::RaytracerCPU(SDL_Window* window) : Raytracer(window) {
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

        //Init Texure
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::vector<unsigned char> buffer = std::vector<unsigned char>(screenWidth * 3 * screenHeight);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RaytracerCPU::RenderSceneToWindow(Scene& scene) const {
        textureShader.Activate();
        textureShader.SetInt("tex", 0);

        std::vector<unsigned char> buffer = RenderSceneToBuffer(scene);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    std::vector<unsigned char> RaytracerCPU::RenderSceneToBuffer(Scene& scene) const {
        std::vector<unsigned char> buffer = std::vector<unsigned char>(screenWidth * 3 * screenHeight);
        float invWidth = 1 / float(screenWidth), invHeight = 1 / float(screenHeight);
        float fov = 30;
        float aspectratio = screenWidth / float(screenHeight);
        float angle = tan(M_PI * 0.5 * fov / 180.0f);

        glm::vec4 camPosWorld = scene.GetCamera().GetTransform().GetTransformMatrix() * glm::vec4(0, 0, 0, 1);

#pragma omp parallel for schedule(runtime)
        for (int x = 0; x < screenWidth; x++) {
            for (int y = 0; y < screenHeight; y++) {
                float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
                float yy = -(1 - 2 * ((y + 0.5) * invHeight)) * angle;  //Y-Axis is flipped

                //Calculating the camera ray with camera transformations in mind
                glm::vec4 projectionRayWorld = scene.GetCamera().GetTransform().GetTransformMatrix() * glm::vec4(xx, yy, 1, 1);
                glm::vec4 projectionRayDirWorld = projectionRayWorld - camPosWorld;
                glm::vec3 camPosWorldV3 = glm::vec3(camPosWorld.x, camPosWorld.y, camPosWorld.z);
                glm::vec3 projectionRayDirWorldV3 = glm::vec3(projectionRayDirWorld.x, projectionRayDirWorld.y, projectionRayDirWorld.z);
                projectionRayDirWorldV3 = glm::normalize(projectionRayDirWorldV3);

                //Actual raytracing
                glm::vec3 pixelColor = Raytrace(scene, camPosWorldV3, projectionRayDirWorldV3, 0);

                buffer[(x + y * screenWidth) * 3] = (unsigned char)(pixelColor.r * 255.0f);
                buffer[(x + y * screenWidth) * 3 + 1] = (unsigned char)(pixelColor.g * 255.0f);
                buffer[(x + y * screenWidth) * 3 + 2] = (unsigned char)(pixelColor.b * 255.0f);
            }
        }
        return buffer;
    }

    glm::vec3 RaytracerCPU::Raytrace(Scene& scene, glm::vec3& origin, glm::vec3& dir, int depth) const {
        Math::IntersectionData intersect = RaycastObjects(scene.GetRenderableObjects(), origin, dir);
        glm::vec3 surfaceColor = glm::vec3(0, 0, 0);
        if (intersect.IsHit()) {
            if (depth < 5 && (intersect.GetMaterial().GetReflectivity() > 0.0f || intersect.GetMaterial().GetTransparency() > 0.0f)) {
                //Reflective and refractive color computation
                glm::vec3 intersectPos = intersect.GetIntersectionPos();
                bool inObject = false;
                glm::vec3 norm = intersect.GetIntersectionNormal();
                if (glm::dot(dir, norm) > 0) {
                    norm = -norm;
                    inObject = true;
                }

                float facingRatio = -glm::dot(dir, norm);
                float fresnel = glm::mix((float)std::pow(1 - facingRatio, 3), 1.0f, 0.1f);

                glm::vec3 reflectionDir = dir - norm * 2.0f * glm::dot(dir, norm);

                reflectionDir = glm::normalize(reflectionDir);

                glm::vec3 newRayOrigin = intersectPos + norm * 0.0001f;
                glm::vec3 reflectionColor = Raytrace(scene, newRayOrigin, reflectionDir, depth + 1);
                glm::vec3 refractionColor = glm::vec3(0, 0, 0);

                if (intersect.GetMaterial().GetTransparency() > 0.0f) {
                    float ior = 1.1f;  //Index of refraction
                    float eta = ior;
                    if (!inObject) {
                        eta = 1.0f / ior;
                    }
                    float cosi = -glm::dot(dir, norm);
                    float k = 1 - eta * eta * (1 - cosi * cosi);
                    glm::vec3 refractionDir = dir * eta + norm * (eta * cosi - std::sqrt(k));
                    refractionDir = glm::normalize(refractionDir);
                    newRayOrigin = intersectPos - norm * 0.0001f;
                    refractionColor = Raytrace(scene, newRayOrigin, refractionDir, depth + 1);
                }

                surfaceColor = (reflectionColor * fresnel * intersect.GetMaterial().GetReflectivity() + refractionColor * (1 - fresnel) * intersect.GetMaterial().GetTransparency()) * intersect.GetMaterial().GetColor();
            } else {
                //Diffuse color computation
                for (std::unique_ptr<Objects::PointLight>& pointLight : scene.GetPointLights()) {
                    float dst = glm::distance(intersect.GetIntersectionPos(), pointLight->GetTransform().GetPosition());
                    if (dst < 1.0f) {
                        dst = 1.0f;
                    }

                    glm::vec3 shadowRayDir = pointLight->GetTransform().GetPosition() - intersect.GetIntersectionPos();
                    glm::vec3 norm = intersect.GetIntersectionNormal();
                    shadowRayDir = glm::normalize(shadowRayDir);

                    glm::vec3 newRayOrigin = intersect.GetIntersectionPos() + norm * 0.0001f;
                    Math::IntersectionData shadowIntersect = RaycastObjects(scene.GetRenderableObjects(), newRayOrigin, shadowRayDir);

                    if (!shadowIntersect.IsHit()) {
                        //Diffuse color calculation. Light intensity uses the inverse square law with a scaling modifier
                        surfaceColor += intersect.GetMaterial().GetColor() * std::max(0.0f, glm::dot(norm, shadowRayDir)) * pointLight->GetColor() * pointLight->GetIntensity() * 1.0f / std::pow(dst, 0.01f);
                    }
                }
            }
        }
        return surfaceColor;
    }

    Tracer::Math::IntersectionData RaytracerCPU::RaycastObjects(std::vector<std::unique_ptr<Objects::RenderableObject>>& renderableObjects, glm::vec3& origin, glm::vec3& dir) const {
        Tracer::Math::IntersectionData closestIntersect = Math::IntersectionData();
        float closesIntersectDst = 99999999.9f;
        for (std::unique_ptr<Objects::RenderableObject>& renderableObject : renderableObjects) {
            Tracer::Math::IntersectionData intersect = renderableObject->Intersect(origin, dir);
            float dst = glm::distance(intersect.GetIntersectionPos(), origin);
            if (intersect.IsHit() && dst < closesIntersectDst) {
                closesIntersectDst = dst;
                closestIntersect = intersect;
            }
        }
        return closestIntersect;
    }
}  // namespace Tracer::Rendering