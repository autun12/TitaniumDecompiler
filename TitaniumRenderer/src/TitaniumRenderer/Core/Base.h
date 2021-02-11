#pragma once

#include <memory>

#include "TitaniumRenderer/Core/PlatformDetection.h"

// Debug settings
#ifdef TD_DEBUG
#if defined(TD_PLATFORM_WINDOWS)
#define TD_DEBUGBREAK() __debugbreak()
#elif defined(TD_PLATFORM_LINUX)
#include <signal.h>
#define TD_DEBUGBREAK() raise(SIGTRAP)
#else
#endif
#define TD_ENABLE_ASSERTS
#else
#define TD_DEBUGBREAK()
#endif

#define TD_EXPAND_MACRO(x) x
#define TD_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define TD_BIND_EVENT_FN(fn)                                    \
    [this](auto&&... args) -> decltype(auto) {                  \
        return this->fn(std::forward<decltype(args)>(args)...); \
    }

namespace TitaniumRenderer {

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}  // namespace TitaniumRenderer

#include "TitaniumRenderer/Core/Assert.h"
#include "TitaniumRenderer/Core/Log.h"