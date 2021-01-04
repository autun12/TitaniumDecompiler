#pragma once

extern TitaniumRenderer::Application* TitaniumRenderer::CreateApplication();

int main(int argc, char** argv) {
	TitaniumRenderer::Log::Init();
	auto app = TitaniumRenderer::CreateApplication();
	app->Run();
	delete app;
}