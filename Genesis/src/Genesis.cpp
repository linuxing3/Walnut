#include "GameLayer.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

using namespace Genesis;

static std::shared_ptr<GameLayer> s_GameLayer;

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
  Walnut::ApplicationSpecification spec;
  spec.Name = "Walnut Example";
  spec.CustomTitlebar = true;

  Walnut::Application* app = new Walnut::Application(spec);
  // NOTE:
  std::shared_ptr<GameLayer> gameLayer = std::make_shared<GameLayer>();
  app->PushLayer(gameLayer);
  s_GameLayer = gameLayer;

  app->SetMenubarCallback([app, gameLayer]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) {
      }
      ImGui::EndMenu();
    }
  });
  return app;
}