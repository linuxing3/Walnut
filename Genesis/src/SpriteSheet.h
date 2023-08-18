#pragma once

#include <cstdint>

#include <string>

class SpriteSheet {
 public:
  SpriteSheet() = delete;
  SpriteSheet(const std::string path);
  ~SpriteSheet() = default;

  void load();

  uint32_t GetWidth() { return m_Width; };
  uint32_t GetHeight() { return m_Height; };
  uint32_t* GetPixels() { return m_SpiritPixels; };

 private:
  std::string m_Filepath;
  uint32_t m_Width, m_Height = 0;
  uint32_t* m_SpiritPixels;
};
