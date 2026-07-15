#pragma once

#include <TitaniumLogger.h>
extern TitaniumRenderer::Application* TitaniumRenderer::CreateApplication();

int main(int argc, char** argv) {
	TitaniumLogger::Log::Init();
	auto app = TitaniumRenderer::CreateApplication();
	app->Run();
	delete app;
}