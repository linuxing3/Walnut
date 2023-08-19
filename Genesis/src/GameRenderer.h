#pragma once

#include "Walnut/Image.h"

#include <cstdint>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <memory>

class GameRenderer {
 public:
  GameRenderer(uint32_t width, uint32_t height);

  void OnResize(uint32_t width, uint32_t height);
  void RenderSprite(uint32_t cx, uint32_t cy);
  void Clear();
  void Update();
  void ClearSingleColor(uint32_t color = 0xff7f007f);

  uint32_t *GetImageData() { return m_ImageData; };
  [[nodiscard]] std::shared_ptr<Walnut::Image> GetFinalImage() const {
    return m_FinalImage;
  }

  uint32_t GetWidth() { return m_FinalImage->GetWidth(); };
  uint32_t GetHeight() { return m_FinalImage->GetHeight(); };

 private:
  std::shared_ptr<Walnut::Image> m_FinalImage;
  uint32_t *m_ImageData = nullptr;
};
