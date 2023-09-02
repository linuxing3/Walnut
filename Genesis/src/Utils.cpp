#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>

namespace Utils {
struct Encoder {
  std::string m_FilePath;
  uint32_t* data;

  Encoder() = default;
  ~Encoder() = default;
  Encoder(const std::string& filePath);
};
}  // namespace Utils
