#pragma once

#include "TitaniumRenderer/Core/Base.h"
namespace TitaniumRenderer {

class GraphicsContext {
public:
    virtual ~GraphicsContext() = default;

    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    static Scope<GraphicsContext> Create(void* window);
};

}  // namespace TitaniumRenderer
