#include "GameLayer.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
  Walnut::ApplicationSpecification spec;
  spec.Name = "Walnut Example";
  spec.CustomTitlebar = true;

  Walnut::Application* app = new Walnut::Application(spec);
  // NOTE:
  std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();
  app->PushLayer(exampleLayer);

  app->SetMenubarCallback([app, exampleLayer]() {
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