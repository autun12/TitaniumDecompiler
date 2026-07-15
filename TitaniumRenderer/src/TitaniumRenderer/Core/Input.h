#pragma once

// #include <glm/glm.hpp>
#include "TitaniumRenderer/Core/KeyCodes.h"
#include "TitaniumRenderer/Core/MouseCodes.h"

namespace TitaniumRenderer {
struct MousePos {
    float x;
    float y;
};

class Input {
public:
    static bool IsKeyPressed(KeyCode key);

    static bool IsMouseButtonPressed(MouseCode button);
    static MousePos GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
};

}  // namespace TitaniumRenderer