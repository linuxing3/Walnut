#include <algorithm>
#include <cstdint>

#include <execution>
#include <iostream>
#include <memory>

#include "GameRenderer.h"
#include "SpriteSheet.h"
#include "GameLevel.h"
#include "Walnut/Image.h"
#include "Walnut/Layer.h"
#include "Walnut/Random.h"
#include "Walnut/UI/UI.h"

#include "Walnut/Timer.h"

namespace Genesis {

class GameLayer : public Walnut::Layer {
 public:
  static std::shared_ptr<GameLayer> Get();

 public:
  GameLayer() {
    m_SpriteSheet = std::make_shared<SpriteSheet>("spaceship.png");
    m_Renderer = std::make_shared<GameRenderer>(800, 600);
    m_Level = std::make_shared<Level>("rock.png");
    m_LevelBackground = std::make_shared<Level>("background.jpg");
  };

  virtual void OnUpdate(float ts) override {
    m_Renderer->Clear();
    
    m_LevelBackground->RenderBackground(m_Renderer);
   
    m_Level->Render(m_Renderer);

    // m_Level->RenderTiles(4, 1, m_Renderer);

    m_Renderer->RenderSprite(4, 1);
  }

  virtual void OnUIRender() override {
    ImGui::Begin("Game");

    // Backend Data (should be in layer renderer)
    m_WindowWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
    m_WindowHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

    // Frontend UI (in ui layer)
    auto finalImage = m_Renderer->GetFinalImage();
    if (finalImage) {
      ImGui::Image(finalImage->GetDescriptorSet(),
                   ImVec2((float)finalImage->GetWidth(),
                          (float)finalImage->GetHeight()));
    }
    ImGui::End();

    render();
  }

  void render() {
    Walnut::Timer timer;
    m_LastRenderTime = timer.ElapsedMillis();
    resize();
  }

  void resize() { m_Renderer->OnResize(m_WindowWidth, m_WindowHeight); }

  std::shared_ptr<SpriteSheet> GetSpriteSheet() { return m_SpriteSheet; };

 private:
  uint32_t m_WindowWidth = 0;
  uint32_t m_WindowHeight = 0;

  std::shared_ptr<GameRenderer> m_Renderer;
  std::shared_ptr<SpriteSheet> m_SpriteSheet;
  std::shared_ptr<Level> m_Level;
  std::shared_ptr<Level> m_LevelBackground;

  float m_LastRenderTime = 0.0F;
};

}  // namespace Genesis