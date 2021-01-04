#include <TitaniumRenderer.h>
#include <TitaniumRenderer/EntryPoint.h>

#include "TitaniumLayer.h"

namespace TitaniumRenderer {
class TitaniumDecompilerApplication : public Application {
public:
	TitaniumDecompilerApplication() : Application("Titanium Decompiler") {
		PushLayer(new TitaniumLayer());
	}

	~TitaniumDecompilerApplication() {}
};

Application* CreateApplication() {
	return new TitaniumDecompilerApplication();
}

}
