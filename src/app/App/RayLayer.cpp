#include <functional>
#include <stdexcept>

#include <imgui_internal.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <Core/Image.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdint.h>

#include "RayLayer.h"

namespace Walnut {
RayLayer::ApplicationLayer()
    : m_logger{spdlog::stdout_color_st("ApplicationLayer")} {}

void RayLayer::OnUIRenderT() {}

void RayLayer::OnUIRender() {}
}  // namespace Walnut
