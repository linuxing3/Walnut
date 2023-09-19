
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "SparkLayer.h"
#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
  Walnut::ApplicationSpecification spec;
  spec.Name = "Walnut Example";
  spec.CustomTitlebar = true;

  Walnut::Application* app = new Walnut::Application(spec);
  // NOTE:
  std::shared_ptr<SparkLayer> sparkLayer = std::make_shared<SparkLayer>();
  app->PushLayer(sparkLayer);

  app->SetMenubarCallback([app, sparkLayer]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("About")) {
        sparkLayer->ShowAboutModal();
      }
      ImGui::EndMenu();
    }
  });
  return app;
}
