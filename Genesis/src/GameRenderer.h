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
  void Render(uint32_t cx, uint32_t cy);
  void Clear();

  uint32_t *GetImageData() { return m_ImageData; };
  std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }


 private:
  std::shared_ptr<Walnut::Image> m_FinalImage;
  uint32_t *m_ImageData = nullptr;

};
