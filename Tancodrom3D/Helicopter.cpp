#include "Helicopter.h"
#include <glfw3.h>
#include <iostream>

Helicopter::Helicopter(string const& path, bool bSmoothNormals, bool gamma) :
    Model(path, bSmoothNormals, gamma)
{
}

void Helicopter::SetRootTransf(glm::mat4 rootTransf)
{
    _rootTransf = rootTransf;
}

void Helicopter::Draw(Shader& shader)
{
    shader.setMat4("model", _rootTransf); // Default transformation for all meshes
    for (unsigned int i = 0; i < meshes.size(); i++) {
        if (meshes[i].name == "main_rotor") {
            double currentFrame = glfwGetTime();
            glm::mat4 rotorModel = _rootTransf;
            glm::vec3 rotorPivot = meshes[i].CalculatePivot(); // Replace with actual pivot

            rotorModel = glm::translate(rotorModel, rotorPivot);
            rotorModel = glm::rotate(rotorModel, (float)(20.0f * currentFrame), glm::vec3(0.0f, 0.0f, 1.0f));
            rotorModel = glm::translate(rotorModel, -rotorPivot);

            shader.setMat4("model", rotorModel);
        }
        else if (meshes[i].name == "rear_rotor") {
            double currentFrame = glfwGetTime();
            glm::mat4 rotorModel = _rootTransf;
            glm::vec3 rotorPivot = meshes[i].CalculatePivot(); // Replace with actual pivot

            rotorModel = glm::translate(rotorModel, rotorPivot);
            rotorModel = glm::rotate(rotorModel, (float)(20.0f * currentFrame), glm::vec3(1.0f, 0.0f, 0.0f));
            rotorModel = glm::translate(rotorModel, -rotorPivot);

            shader.setMat4("model", rotorModel);
        }
        else {
            shader.setMat4("model", _rootTransf); // Reset transformation
        }
        meshes[i].Draw(shader);
    }
}



