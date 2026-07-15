#include "TitaniumRenderer/Core/Base.h"

#include "TitaniumRenderer/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

#ifdef TD_PLATFORM_MACOS
#include "Platform/Metal/MetalRendererAPI.h"
#endif

namespace TitaniumRenderer {
#ifdef TD_PLATFORM_MACOS
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Metal;
#else
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#endif

	Scope<RendererAPI> RendererAPI::Create() {
		switch(s_API) {
			case RendererAPI::API::None:    TD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
			case RendererAPI::API::Metal:  break;
		}

		TD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}