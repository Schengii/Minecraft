#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <glm/glm.hpp>
#include <string>

namespace Minecraft {

struct TransformComponent {
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f };
    glm::vec3 scale{ 1.0f };
};

struct VelocityComponent {
    glm::vec3 velocity{ 0.0f };
};

struct BoundingBoxComponent {
    glm::vec3 minOffset{ -0.3f, 0.0f, -0.3f };
    glm::vec3 maxOffset{  0.3f, 1.8f,  0.3f };
};

struct PlayerComponent {
    bool isGrounded = false;
    float eyeHeight = 1.62f;
    float walkSpeed = 5.0f;
    float flySpeed = 15.0f;
    bool isFlying = true;
};

struct TagComponent {
    std::string tag = "Entity";
};

}

#endif // COMPONENTS_HPP
