#include "TitaniumRenderer/Core/Base.h"
#include "TitaniumRenderer/Renderer/RenderCommand.h"

namespace TitaniumRenderer {
Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}