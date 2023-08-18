#include "GameLevel.h"
#include "GameRenderer.h"
#include "SpriteSheet.h"
#include "Walnut/Random.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/types.h>

namespace Genesis {

Level::Level(const std::filesystem::path& path) : m_Filepath(path) {
  for (uint32_t i = 0; i < m_Width * m_Height; i++) {
    m_Grass[i] = Walnut::Random::UInt(0, 7);
  }
  LoadLevelFromFile(m_Filepath);
};

Level::~Level() {
  stbi_image_free(m_Tiles);
  delete[] m_Grass;
}

void Level::Render(std::shared_ptr<GameRenderer> renderer) {
  uint32_t* buffer = renderer->GetImageData();
  for (uint32_t j = 0; j < m_Height; j++) {
    if (j > renderer->GetHeight())
      break;
    for (uint32_t i = 0; i < m_Width; i++) {
      if (i > renderer->GetWidth())
        break;
      uint32_t tileID = m_Tiles[i + j * m_Width];
      buffer[i + j * renderer->GetWidth()] = tileID;
    }
  }
  
  renderer->GetFinalImage()->SetData(buffer);
};

void Level::LoadLevelFromFile(const std::filesystem::path& path) {
  int width, height, channels;
  std::string pathStr = path.string();
  m_Tiles =
      (uint32_t*)stbi_load(pathStr.c_str(), &width, &height, &channels, 4);

  m_Grass = new uint32_t[width * height];

  m_Width = width;
  m_Height = height;
}

void Level::RenderBackground(std::shared_ptr<GameRenderer> renderer) {

  uint32_t* buffer = renderer->GetImageData();

  uint32_t scale[2] = {m_Width / renderer->GetWidth(),
                       m_Height / renderer->GetHeight()};

  for (uint32_t j = 0; j < renderer->GetHeight(); j++) {
    for (uint32_t i = 0; i < renderer->GetWidth(); i++) {
      buffer[i + j * renderer->GetWidth()] =
         m_Tiles[i / scale[0] + j / scale[1] * m_Width];
    }
  }
  renderer->GetFinalImage()->SetData(buffer);
}

void Level::RenderTiles(int cx, int cy, std::shared_ptr<GameRenderer> renderer) {
  uint32_t* buffer = renderer->GetImageData();
  uint32_t spriteSize = 64;
  uint32_t xp = cx * spriteSize, yp = cy * spriteSize;

  uint32_t nx = renderer->GetWidth() / spriteSize;
  uint32_t ny = renderer->GetHeight() / spriteSize;

  uint32_t xt = 0;
  uint32_t yt = 0;

  for (uint32_t y = 0; y < spriteSize; y++)
    for (uint32_t x = 0; x < spriteSize; x++) {

      uint32_t color = m_Grass[(xp + x) + (yp + y) * m_Width];

      for (uint32_t j = 0; j < ny; j++) {
        for (uint32_t i = 0; i < nx; i++) {
          uint32_t coord_x = xt + x + spriteSize * i;
          uint32_t coord_y = (yt + y + spriteSize * j) * renderer->GetWidth();
          buffer[coord_x + coord_y] = color;
        }
      }
    };
  renderer->GetFinalImage()->SetData(buffer);
}
};  // namespace Genesis
