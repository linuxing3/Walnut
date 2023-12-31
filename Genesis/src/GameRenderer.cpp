#include "GameRenderer.h"
#include "GameObject.h"
#include "SpriteSheet.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "glm/fwd.hpp"
#include <algorithm>
#include <cstdint>
#include <execution>
#include <iostream>
#include <memory>

#include "GameLayer.h"

namespace Utils {

static uint32_t ConvertToRGBA(const glm::vec4 &color) {
  auto r = (uint8_t)(color.r * 255.0f);
  auto g = (uint8_t)(color.g * 255.0f);
  auto b = (uint8_t)(color.b * 255.0f);
  auto a = (uint8_t)(color.a * 255.0f);

  uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
  return result;
}

}  // namespace Utils

GameRenderer::GameRenderer(uint32_t width, uint32_t height) {
  // NOTE: Prepare image data
  m_ImageData = new uint32_t[width * height];
  for (uint32_t i = 0; i < width * height; i++) {
    m_ImageData[i] = 0xffff00ff;
  }
  // NOTE: Prepare Imgui Image object
  if (m_FinalImage) {
    if (m_FinalImage->GetWidth() == width &&
        m_FinalImage->GetHeight() == height)
      return;

    m_FinalImage->Resize(width, height);
  } else {
    m_FinalImage = std::make_shared<Walnut::Image>(width, height,
                                                   Walnut::ImageFormat::RGBA);
  }
};

void GameRenderer::OnResize(uint32_t width, uint32_t height) {
  if (m_FinalImage) {
    /* std::cout << "Checking resize necessity"; */
    if (m_FinalImage->GetWidth() == width &&
        m_FinalImage->GetHeight() == height)
      return;

    m_FinalImage->Resize(width, height);
  } else {
    /* std::cout << "Creating Walnut::Image"; */
    m_FinalImage = std::make_shared<Walnut::Image>(width, height,
                                                   Walnut::ImageFormat::RGBA);
  }

  // NOTE: Each time rezize, image data array must be rebuilt
  delete[] m_ImageData;
  m_ImageData = new uint32_t[width * height];

  m_WindowHeight = height;
  m_WindowWidth = width;
}

void GameRenderer::RenderSprite(uint32_t cx, uint32_t cy) {
  // Spritsheet
  auto ss = Genesis::GameLayer::Get()->GetPlayerObject();
  /*
   *   Spritsheet is a grid with each subimage in cell
   *                     cy
   *  cx -----------------
   *    |   |   |   |   |
   *     -----------------
   *    |   |   |   |   |
   *     -----------------
   *    |   |ss |   |   |
   *     -----------------
   *    |   |   |   |   |
   *    -----------------
   *  (0, 0)
   */
  uint32_t xp = cx * spriteSize, yp = cy * spriteSize;

  for (uint32_t y = 0; y < spriteSize; y++)
    for (uint32_t x = 0; x < spriteSize; x++) {
      /*
       *  Subimage grid data is like matrix
       *                    y
       *  x -----------------
       *    | 0 |   |   |   |
       *    -----------------
       *    |   |   |   |   |
       *    -----------------
       *    |   | 9 |   |   |
       *    -----------------
       *    |   |   |   |   |
       *    -----------------
       *  (xp, yp)
       *
       *    Flattend to one dimension array, index_within_image
       *    ----------------- -------------------------------
       *    | 0 |   |   |   |   |   |   |   |  |  9 |   |   |
       *    ----------------- -------------------------------
       */
      uint32_t index_within_image = (xp + x) + (yp + y) * ss->GetWidth();
      uint32_t color = ss->GetPixels()[index_within_image];
      if (color == 0xff000000 || color == 0xffff00ff)
        continue;

      uint32_t index_within_viewport =
          (xt + x) + m_FinalImage->GetWidth() * (yt + y);
      m_ImageData[index_within_viewport] = color;
    };

  m_FinalImage->SetData(m_ImageData);
}

// TODO: transform and movement
void GameRenderer::Update(float ts) {}

void GameRenderer::Clear() {
  for (uint32_t i = 0; i < m_FinalImage->GetWidth() * m_FinalImage->GetHeight();
       i++) {
    m_ImageData[i] = Walnut::Random::UInt();
  }
}

void GameRenderer::ClearSingleColor(uint32_t color) {
  for (uint32_t i = 0; i < m_FinalImage->GetWidth() * m_FinalImage->GetHeight();
       i++) {
    m_ImageData[i] = color;
  }
}
