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
  // get pixels of renderer image
  uint32_t* buffer = renderer->GetImageData();
  for (uint32_t j = 0; j < m_Height; j++) {
    if (j > renderer->GetHeight())
      break;
    for (uint32_t i = 0; i < m_Width; i++) {
      if (i > renderer->GetWidth())
        break;
      // get color at pixel coordinate of m_Tiles image
      uint32_t tileID = m_Tiles[i + j * m_Width];
      // write to renderer image pixel coordinate
      buffer[i + j * renderer->GetWidth()] = tileID;
    }
  }
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

  // NOTE: Background image size > Renderer viewport size, we calculate how to
  // fit in
  uint32_t scale[2] = {m_Width / renderer->GetWidth(),
                       m_Height / renderer->GetHeight()};

  for (uint32_t j = 0; j < renderer->GetHeight(); j++) {
    for (uint32_t i = 0; i < renderer->GetWidth(); i++) {
      uint32_t index_within_viewport = i + j * renderer->GetWidth();
      uint32_t index_within_image = i / scale[0] + m_Width * j / scale[1];
      buffer[index_within_viewport] = m_Tiles[index_within_image];
    }
  }
}

void Level::RenderTiles(int cx, int cy,
                        std::shared_ptr<GameRenderer> renderer) {
  uint32_t* buffer = renderer->GetImageData();
  uint32_t spriteSize = 64;

  uint32_t x_position_of_sprite_within_sheet = cx * spriteSize;
  uint32_t y_position_of_sprite_within_sheet = cy * spriteSize;

  // NOTE: Tile image size < Renderer Viewport size, we calculate how many quads
  uint32_t tiles_perline = renderer->GetWidth() / spriteSize;
  uint32_t tiles_percolumn = renderer->GetHeight() / spriteSize;

  uint32_t sprite_start_anchor_x = 0;
  uint32_t sprite_start_anchor_y = 0;

  for (uint32_t y_coord_within_sprite = 0; y_coord_within_sprite < spriteSize;
       y_coord_within_sprite++)
    for (uint32_t x_coord_within_sprite = 0; x_coord_within_sprite < spriteSize;
         x_coord_within_sprite++) {

      uint32_t color =
          m_Grass[(x_position_of_sprite_within_sheet + x_coord_within_sprite) +
                  (y_position_of_sprite_within_sheet + y_coord_within_sprite) *
                      m_Width];

      for (uint32_t j = 0; j < tiles_percolumn; j++) {
        for (uint32_t i = 0; i < tiles_perline; i++) {
          uint32_t x_coord =
              sprite_start_anchor_x + x_coord_within_sprite + spriteSize * i;
          uint32_t y_coord =
              (sprite_start_anchor_y + y_coord_within_sprite + spriteSize * j) *
              renderer->GetWidth();
          buffer[x_coord + y_coord] = color;
        }
      }
    };
}
};  // namespace Genesis
