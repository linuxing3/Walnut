#pragma once

#include "Walnut/Image.h"

#include <cstdint>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <memory>

class GameRenderer {
 public:
  uint32_t xt = 0;
  uint32_t yt = 0;
  uint32_t spriteSize = 40;
  uint32_t spritePadding = 10;

 public:
  GameRenderer(uint32_t width, uint32_t height);

  void OnResize(uint32_t width, uint32_t height);

  void RenderSprite(uint32_t cx, uint32_t cy);

  void Clear();
  void Update(float ts);
  void ClearSingleColor(uint32_t color = 0xff7f007f);

  uint32_t *GetImageData() { return m_ImageData; };
  [[nodiscard]] std::shared_ptr<Walnut::Image> GetFinalImage() const {
    return m_FinalImage;
  }

  uint32_t GetWidth() { return m_FinalImage->GetWidth(); };
  uint32_t GetHeight() { return m_FinalImage->GetHeight(); };
  uint32_t SetStartPosition(uint32_t _xt = 0, uint32_t _yt = 0) {
    xt = _xt;
    yt = _yt;
  };

 private:
  std::shared_ptr<Walnut::Image> m_FinalImage;
  uint32_t *m_ImageData = nullptr;
};
