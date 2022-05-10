#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <pugixml.hpp>
#include <vector>

#include "camera.hpp"
#include "meshObject.hpp"
#include "pointLight.hpp"
#include "renderableObject.hpp"
#include "sphere.hpp"

namespace Tracer::Rendering {
    class Scene {
    private:
        std::vector<std::unique_ptr<Objects::RenderableObject>> renderableObjects;
        std::vector<std::unique_ptr<Objects::MeshObject>> meshObjects;
        std::vector<std::unique_ptr<Objects::Sphere>> spheres;
        std::vector<std::unique_ptr<Objects::PointLight>> pointLights;
        Objects::Camera camera;
        bool rotateCameraAroundOrigin = false;

    public:
        Scene();

        void LoadSceneDataFromFile(std::string path);

        void AddMeshObject(Objects::MeshObject& meshObject);

        void AddSphere(Objects::Sphere& sphere);

        void AddPointLight(Objects::PointLight& pointLight);

        void SetCamera(Objects::Camera& camera);

        void SetCameraRotateAroundOrigin(bool value);

        void Update(float delta);

        std::vector<std::unique_ptr<Objects::RenderableObject>>& GetRenderableObjects();

        std::vector<std::unique_ptr<Objects::MeshObject>>& GetMeshObjects();

        std::vector<std::unique_ptr<Objects::Sphere>>& GetSpheres();

        std::vector<std::unique_ptr<Objects::PointLight>>& GetPointLights();

        Objects::Camera& GetCamera();

        bool GetCameraRotateAroundOrigin();
    };
}  // namespace Tracer::Rendering