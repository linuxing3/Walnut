#pragma once

#include "SpriteSheet.h"

/* #include <corecrt_math.h> */
#include <cstdint>

#include <iostream>
#include <string>

#define stbi__vertically_flip_on_load
#include "stb_image.h"

SpriteSheet::SpriteSheet(const std::string path) : m_Filepath(path) {
  load();
}

void SpriteSheet::load() {
  int width, height, channels;
  if (stbi_is_hdr(m_Filepath.c_str())) {
    m_SpiritPixels = (uint32_t*)stbi_loadf(m_Filepath.c_str(), &width, &height,
                                           &channels, 4);
  } else {
    m_SpiritPixels =
        (uint32_t*)stbi_load(m_Filepath.c_str(), &width, &height, &channels, 4);
  }

  m_Width = width;
  m_Height = height;
}
