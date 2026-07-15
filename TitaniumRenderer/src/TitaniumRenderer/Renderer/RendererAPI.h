#pragma once

// #include <glm/glm.hpp>

namespace TitaniumRenderer {
struct Color {
    float r;
    float g;
    float b;
    float a;
};

class RendererAPI {
public:
    enum class API {
        None = 0, OpenGL = 1, Metal = 2
    };

public:
    virtual ~RendererAPI() = default;
    virtual void Init() = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void SetClearColor(const Color& color) = 0;
    virtual void Clear() = 0;
    static API GetAPI() { return s_API; }
    static Scope<RendererAPI> Create();
private:
    static API s_API;
};
}