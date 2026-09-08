#pragma once

#include "TitaniumRenderer/Renderer/RendererAPI.h"

namespace MTL {
class DepthStencilState;
}

namespace TitaniumRenderer {
class MetalRendererAPI : public RendererAPI {
public:
    virtual void Init() override;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                             uint32_t height) override;
    virtual void SetClearColor(const Color& color) override;
    virtual void Clear() override;

private:
    MTL::DepthStencilState* m_DepthStencilState = nullptr;

    double m_ClearColor[4]{0.1, 0.1, 0.1, 1.0};

    struct ViewportBounds {
        double x = 0.0;
        double y = 0.0;
        double width = 1280.0;
        double height = 720.0;
        double znear = 0.0;
        double zfar = 1.0;
    } m_Viewport;
};

}  // namespace TitaniumRenderer
