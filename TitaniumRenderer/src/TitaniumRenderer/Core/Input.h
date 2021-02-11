#pragma once

#include <glm/glm.hpp>
#include "TitaniumRenderer/Core/KeyCodes.h"
#include "TitaniumRenderer/Core/MouseCodes.h"

namespace TitaniumRenderer {

class Input {
public:
    static bool IsKeyPressed(KeyCode key);

    static bool IsMouseButtonPressed(MouseCode button);
    static glm::vec2 GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
};

}  // namespace TitaniumRenderer