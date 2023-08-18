#include "GameRenderer.h"

#include "SpriteSheet.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
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
  m_ImageData = new uint32_t[width * height];
  for (uint32_t i = 0; i < width * height; i++) {
    m_ImageData[i] = 0xffff00ff;
  }
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

  delete[] m_ImageData;
  m_ImageData = new uint32_t[width * height];
}

void GameRenderer::RenderSprite(uint32_t cx, uint32_t cy) {
  // Spritsheet
  auto ss = Genesis::GameLayer::Get()->GetSpriteSheet();
  uint32_t spriteSize = 40;
  uint32_t spritePadding = 10;
  uint32_t xp = cx * spriteSize, yp = cy * spriteSize;

  uint32_t middleX = m_FinalImage->GetWidth() / 2;
  uint32_t middleY = m_FinalImage->GetHeight() * 3 / 4;

  uint32_t xt = (middleX - spriteSize / 2);
  uint32_t yt = (middleY - spriteSize / 2);

  for (uint32_t y = 0; y < spriteSize; y++)
    for (uint32_t x = 0; x < spriteSize; x++) {
      uint32_t color = ss->GetPixels()[(xp + x) + (yp + y) * ss->GetWidth()];

      for (uint32_t i = 0; i < 1; i++) {
        m_ImageData[(xt + x + (spriteSize + spritePadding) * i) +
                    (yt + y) * m_FinalImage->GetWidth()] = color;
      }
    };

  m_FinalImage->SetData(m_ImageData);
}

void GameRenderer::Update() {
  if (m_ImageData) {
    GetFinalImage()->SetData(m_ImageData);
  }
}

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
