#include "GameRenderer.h"
#include "stb_image.h"
#include <filesystem>
#include <iostream>
#include <memory>

namespace Genesis {
class Level {
 public:
  Level(const std::filesystem::path& path);
  ~Level();
  void LoadLevelFromFile(const std::filesystem::path& path);

  void RenderTiles(int cx, int cy, std::shared_ptr<GameRenderer> renderer);
  void Render(std::shared_ptr<GameRenderer> renderer);
  void RenderBackground(std::shared_ptr<GameRenderer> renderer);

 private:
  std::filesystem::path m_Filepath;
  uint32_t m_Width, m_Height = 0;
  uint32_t* m_Tiles = nullptr;
  uint32_t* m_Grass = nullptr;
};
}  // namespace Genesis