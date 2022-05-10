#include "scene.hpp"

namespace Tracer::Rendering {
    Scene::Scene() {}

    void Scene::LoadSceneDataFromFile(std::string path) {
        pugi::xml_document xmlDoc;
        if(xmlDoc.load_file(path.c_str()).status != pugi::xml_parse_status::status_ok) {
            std::cout << "Unable to load scene file.\n";
            exit(1);
        }
        pugi::xml_node sceneNode = xmlDoc.child("CPP_TracerScene");

        //Spheres
        for (pugi::xml_node sphereNode : sceneNode.child("Objects").child("RenderableObjects").child("Spheres").children("Sphere")) {
            glm::vec3 position;
            position.x = sphereNode.child("Position").attribute("x").as_float();
            position.y = sphereNode.child("Position").attribute("y").as_float();
            position.z = sphereNode.child("Position").attribute("z").as_float();

            glm::vec3 color;
            color.x = sphereNode.child("Material").child("Color").attribute("r").as_float();
            color.y = sphereNode.child("Material").child("Color").attribute("g").as_float();
            color.z = sphereNode.child("Material").child("Color").attribute("b").as_float();

            float reflectivity = sphereNode.child("Material").attribute("reflectivity").as_float();
            float transparency = sphereNode.child("Material").attribute("transparency").as_float();

            float radius = sphereNode.attribute("radius").as_float();

            Components::Material material = Components::Material(color, reflectivity, transparency);
            Objects::Sphere sphere = Objects::Sphere(position, material, radius);
            AddSphere(sphere);
        }

        //MeshObjects
        for (pugi::xml_node meshObjectNode : sceneNode.child("Objects").child("RenderableObjects").child("MeshObjects").children("MeshObject")) {
            glm::vec3 position;
            position.x = meshObjectNode.child("Transform").child("Position").attribute("x").as_float();
            position.y = meshObjectNode.child("Transform").child("Position").attribute("y").as_float();
            position.z = meshObjectNode.child("Transform").child("Position").attribute("z").as_float();

            glm::vec3 rotation;
            rotation.x = meshObjectNode.child("Transform").child("Rotation").attribute("x").as_float();
            rotation.y = meshObjectNode.child("Transform").child("Rotation").attribute("y").as_float();
            rotation.z = meshObjectNode.child("Transform").child("Rotation").attribute("z").as_float();

            glm::vec3 scale;
            scale.x = meshObjectNode.child("Transform").child("Scale").attribute("x").as_float();
            scale.y = meshObjectNode.child("Transform").child("Scale").attribute("y").as_float();
            scale.z = meshObjectNode.child("Transform").child("Scale").attribute("z").as_float();

            glm::vec3 color;
            color.x = meshObjectNode.child("Material").child("Color").attribute("r").as_float();
            color.y = meshObjectNode.child("Material").child("Color").attribute("g").as_float();
            color.z = meshObjectNode.child("Material").child("Color").attribute("b").as_float();

            float reflectivity = meshObjectNode.child("Material").attribute("reflectivity").as_float();
            float transparency = meshObjectNode.child("Material").attribute("transparency").as_float();

            Components::Material material = Components::Material(color, reflectivity, transparency);

            Components::Mesh mesh;
            std::string relativeMeshPath = meshObjectNode.attribute("modelPath").as_string();
            std::filesystem::path testPath = std::filesystem::path(relativeMeshPath);
            testPath = std::filesystem::weakly_canonical(testPath);
            mesh.LoadFromObjectFile(testPath);
            Objects::MeshObject meshObject = Objects::MeshObject(position, material, mesh);
            meshObject.GetTransform().SetRotation(rotation);
            meshObject.GetTransform().SetScale(scale);
            AddMeshObject(meshObject);
        }

        //PointLights
        for (pugi::xml_node pointLightNode : sceneNode.child("Objects").child("PointLights").children("PointLight")) {
            glm::vec3 position;
            position.x = pointLightNode.child("Position").attribute("x").as_float();
            position.y = pointLightNode.child("Position").attribute("y").as_float();
            position.z = pointLightNode.child("Position").attribute("z").as_float();

            glm::vec3 color;
            color.x = pointLightNode.child("Color").attribute("r").as_float();
            color.y = pointLightNode.child("Color").attribute("g").as_float();
            color.z = pointLightNode.child("Color").attribute("b").as_float();

            float intensity = pointLightNode.attribute("intensity").as_float();

            Objects::PointLight pointLight = Objects::PointLight(position, color, intensity);
            AddPointLight(pointLight);
        }

        //Camera
        pugi::xml_node cameraNode = sceneNode.child("Objects").child("Camera");

        glm::vec3 position;
        position.x = cameraNode.child("Transform").child("Position").attribute("x").as_float();
        position.y = cameraNode.child("Transform").child("Position").attribute("y").as_float();
        position.z = cameraNode.child("Transform").child("Position").attribute("z").as_float();

        glm::vec3 rotation;
        rotation.x = cameraNode.child("Transform").child("Rotation").attribute("x").as_float();
        rotation.y = cameraNode.child("Transform").child("Rotation").attribute("y").as_float();
        rotation.z = cameraNode.child("Transform").child("Rotation").attribute("z").as_float();

        Objects::Camera camera = Objects::Camera(position, rotation);
        SetCamera(camera);

        rotateCameraAroundOrigin = sceneNode.attribute("rotateCameraAroundOrigin").as_bool();
    }

    void Scene::AddMeshObject(Objects::MeshObject& meshObject) {
        meshObjects.push_back(std::make_unique<Objects::MeshObject>(meshObject));
        renderableObjects.push_back(std::make_unique<Objects::MeshObject>(meshObject));
    }

    void Scene::AddSphere(Objects::Sphere& sphere) {
        spheres.push_back(std::make_unique<Objects::Sphere>(sphere));
        renderableObjects.push_back(std::make_unique<Objects::Sphere>(sphere));
    }

    void Scene::AddPointLight(Objects::PointLight& pointLight) {
        pointLights.push_back(std::make_unique<Objects::PointLight>(pointLight));
    }

    void Scene::SetCamera(Objects::Camera& camera) {
        this->camera = camera;
    }

    void Scene::SetCameraRotateAroundOrigin(bool value) {
        rotateCameraAroundOrigin = true;
    }

    void Scene::Update(float delta) {
        if (rotateCameraAroundOrigin) {
            //When rotating around the the origin the position of a transform and its rotation are not entirely correct!
            GetCamera().GetTransform().RotateAroundOrigin(glm::vec3(0, 1, 0), delta * 20.0f);
        }
    }

    std::vector<std::unique_ptr<Objects::RenderableObject>>& Scene::GetRenderableObjects() {
        return renderableObjects;
    }

    std::vector<std::unique_ptr<Objects::MeshObject>>& Scene::GetMeshObjects() {
        return meshObjects;
    }

    std::vector<std::unique_ptr<Objects::Sphere>>& Scene::GetSpheres() {
        return spheres;
    }

    std::vector<std::unique_ptr<Objects::PointLight>>& Scene::GetPointLights() {
        return pointLights;
    }

    Objects::Camera& Scene::GetCamera() {
        return camera;
    }

    bool Scene::GetCameraRotateAroundOrigin() {
        return rotateCameraAroundOrigin;
    }
}  // namespace Tracer::Rendering