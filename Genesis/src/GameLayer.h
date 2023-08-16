#include <algorithm>
/* #include <basetsd.h> */
#include <cstdint>

#include <execution>
#include <iostream>
#include <memory>

#include "SpriteSheet.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/UI/UI.h"

#include "Walnut/Timer.h"

class ExampleLayer : public Walnut::Layer {
 public:
  ExampleLayer() = default;
  virtual void OnAttach() override {
    // Backend Data
    m_ImageData = new uint32_t[800 * 600];
    for (uint32_t i = 0; i < 800 * 600; i++) {
      m_ImageData[i] = 0xffff00ff;
    }
    m_Image = std::make_shared<Walnut::Image>(
        800, 600, Walnut::ImageFormat::RGBA, m_ImageData);

#ifdef MT
    // Multithreading
    m_VerticalIter.resize(800);
    m_HorizotalIter.resize(600);

    for (uint32_t i = 0; i < 800; i++)
      m_VerticalIter[i] = i;

    for (uint32_t i = 0; i < 600; i++)
      m_HorizotalIter[i] = i;
#endif
  }

  void loadRandomBackground() {
    // #define MT 1

#ifdef MT
    std::for_each(std::execution::par, m_VerticalIter.begin(),
                  m_VerticalIter.end(), [this](uint32_t j) {
                    std::for_each(std::execution::par, m_HorizotalIter.begin(),
                                  m_HorizotalIter.end(), [this, j](uint32_t i) {
                                    m_ImageData[i + j * m_Image->GetWidth()] =
                                        Walnut::Random::UInt();
                                  });
                  });

#else
    for (uint32_t i = 0; i < m_Image->GetWidth() * m_Image->GetHeight(); i++) {
      m_ImageData[i] = Walnut::Random::UInt();
    }
#endif
  }

  void loadImageBackground() {
    // Random Backend Data
    SpriteSheet bg("background.jpg");
    bg.load();
    uint32_t scale[2] = {bg.GetWidth() / m_Image->GetWidth(),
                         bg.GetHeight() / m_Image->GetHeight()};
    uint32_t* pixels = bg.GetPixels();
    uint32_t bg_width = bg.GetWidth();

    // #define MT 1

#ifdef MT
    std::for_each(
        std::execution::par, m_VerticalIter.begin(), m_VerticalIter.end(),
        [this](uint32_t j, uint32_t scale[2], uint32_t* pixels,
               uint32_t bg_width) {
          std::for_each(std::execution::par, m_HorizotalIter.begin(),
                        m_HorizotalIter.end(),
                        [this, j, scale, pixels, bg_width](uint32_t i) {
                          m_ImageData[i + j * m_Image->GetWidth()] =
                              pixels[i * scale[0] + j * scale[1] * bg_width];
                        });
        });

#else
    for (uint32_t j = 0; j < m_Image->GetHeight(); j++) {
      for (uint32_t i = 0; i < m_Image->GetWidth(); i++) {
        m_ImageData[i + j * m_Image->GetWidth()] =
            bg.GetPixels()[i * scale[0] + j * scale[1] * bg.GetWidth()];
      }
    }
#endif
  }

  void loadTile(int cx, int cy) {
    // Spritsheet
    SpriteSheet ss("rock.png");
    ss.load();
    uint32_t spriteSize = 64;
    uint32_t xp = cx * spriteSize, yp = cy * spriteSize;

    uint32_t nx = m_Image->GetWidth() / spriteSize;
    uint32_t ny = m_Image->GetHeight() / spriteSize;

    uint32_t xt = 0;
    uint32_t yt = 0;

    for (uint32_t y = 0; y < spriteSize; y++)
      for (uint32_t x = 0; x < spriteSize; x++) {
        uint32_t color = ss.GetPixels()[(xp + x) + (yp + y) * ss.GetWidth()];

        for (uint32_t j = 0; j < ny; j++) {
          for (uint32_t i = 0; i < nx; i++) {
            uint32_t coord_x = xt + x + spriteSize * i;
            uint32_t coord_y = (yt + y + spriteSize * j) * m_Image->GetWidth();
            m_ImageData[coord_x + coord_y] = color;
          }
        }
      };
  }
  void loadSpaceShip(int cx, int cy) {
    // Spritsheet
    SpriteSheet ss("spaceship.png");
    ss.load();
    uint32_t spriteSize = 40;
    uint32_t spritePadding = 10;
    uint32_t xp = cx * spriteSize, yp = cy * spriteSize;

    uint32_t middleX = m_Image->GetWidth() / 2;
    uint32_t middleY = m_Image->GetHeight() * 3 / 4;

    uint32_t xt = (middleX - spriteSize / 2);
    xt += (uint32_t)m_LastRenderTime * m_Image->GetWidth();
    uint32_t yt = (middleY - spriteSize / 2);
    yt += (uint32_t)m_LastRenderTime * m_Image->GetHeight();

    for (uint32_t y = 0; y < spriteSize; y++)
      for (uint32_t x = 0; x < spriteSize; x++) {
        uint32_t color = ss.GetPixels()[(xp + x) + (yp + y) * ss.GetWidth()];

        for (uint32_t i = 0; i < 2; i++) {
          m_ImageData[(xt + x + (spriteSize + spritePadding) * i) +
                      (yt + y) * m_Image->GetWidth()] = color;
        }
      };
  }
  virtual void OnUpdate(float ts) override {
    // loadImageBackground();
    loadRandomBackground();
    loadTile(5, 0);
    loadSpaceShip(2, 1);

    m_Image->SetData(m_ImageData);
  }

  virtual void OnUIRender() override {
    ImGui::Begin("Game");

    // Backend Data (should be in layer renderer)
    m_WindowWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
    m_WindowHeight = (uint32_t)ImGui::GetContentRegionAvail().y;
    // Frontend UI (in ui layer)
    ImGui::Image(
        m_Image->GetDescriptorSet(),
        ImVec2((float)m_Image->GetWidth(), (float)m_Image->GetHeight()));
    ImGui::End();

    resize();
    render();
  }

  void resize() {
    if (m_WindowHeight != m_Image->GetHeight() ||
        m_WindowWidth != m_Image->GetWidth()) {
      delete[] m_ImageData;
      m_ImageData = new uint32_t[m_WindowWidth * m_WindowHeight];
      m_Image = std::make_shared<Walnut::Image>(m_WindowWidth, m_WindowHeight,
                                                Walnut::ImageFormat::RGBA);
      // Multithreading
      m_VerticalIter.resize(m_WindowHeight);
      m_HorizotalIter.resize(m_WindowWidth);

#ifdef MT
      for (uint32_t i = 0; i < m_WindowHeight; i++)
        m_VerticalIter[i] = i;

      for (uint32_t i = 0; i < m_WindowWidth; i++)
        m_HorizotalIter[i] = i;
#endif  // MT
    }
  }

  void render() {
    Walnut::Timer timer;
    m_LastRenderTime = timer.ElapsedMillis();
  }

 private:
  bool m_AboutModalOpen = false;
  uint32_t m_WindowWidth = 0;
  uint32_t m_WindowHeight = 0;

  std::shared_ptr<Walnut::Image> m_Image;
  uint32_t* m_ImageData = nullptr;
  std::vector<uint32_t> m_VerticalIter;
  std::vector<uint32_t> m_HorizotalIter;
  float m_LastRenderTime = 0.0F;
};
