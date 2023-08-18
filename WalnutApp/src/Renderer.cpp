#include "Renderer.h"

#include "Walnut/Random.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <algorithm>
#include <cstdint>
#include <execution>
#include <iostream>

namespace Utils {

static uint32_t ConvertToRGBA(const glm::vec4 &color) {
  auto r = (uint8_t)(color.r * 255.0f);
  auto g = (uint8_t)(color.g * 255.0f);
  auto b = (uint8_t)(color.b * 255.0f);
  auto a = (uint8_t)(color.a * 255.0f);

  uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
  return result;
}

}  // namespace Utils

void GameRenderer::OnResize(uint32_t width, uint32_t height) {
  if (m_FinalImage) {
    /* std::cout << "Checking resize necessity"; */
    if (m_FinalImage->GetWidth() == width &&
        m_FinalImage->GetHeight() == height)
      return;

    m_FinalImage->Resize(width, height);
  } else {
    /* std::cout << "Creating Walnut::Image"; */
    m_FinalImage = std::make_shared<Walnut::Image>(width, height,
                                                   Walnut::ImageFormat::RGBA);
  }

  delete[] m_ImageData;
  m_ImageData = new uint32_t[width * height];

#ifdef MT

  // Accumulatted sampling
  delete[] m_ImageAccumulatedData;
  m_ImageAccumulatedData = new glm::vec4[width * height];

  // Multithreading
  m_VerticalIter.resize(height);
  m_HorizotalIter.resize(width);

  for (uint32_t i = 0; i < height; i++)
    m_VerticalIter[i] = i;

  for (uint32_t i = 0; i < width; i++)
    m_HorizotalIter[i] = i;
#endif  // MT
}

void GameRenderer::Render(const Scene &scene, const Camera &camera) {
  m_ActiveScene = &scene;
  m_ActiveCamera = &camera;

  /* #define MT 1 */

#ifdef MT

  std::for_each(
      std::execution::par, m_VerticalIter.begin(), m_VerticalIter.end(),
      [this](uint32_t y) {
        std::for_each(
            std::execution::par, m_HorizotalIter.begin(), m_HorizotalIter.end(),
            [this, y](uint32_t x) {
              uint32_t color_index = x + y * m_FinalImage->GetWidth();
              // PIXEL SHADER, but in CPU side
              glm::vec4 color = PerPixel(x, y);
              m_ImageAccumulatedData[color_index] += color;

              glm::vec4 accumulattedColor = m_ImageAccumulatedData[color_index];
              accumulattedColor /= (float)m_FrameIndex;
              accumulattedColor = glm::clamp(accumulattedColor, glm::vec4(0.0f),
                                             glm::vec4(1.0f));
              m_ImageData[color_index] =
                  Utils::ConvertToRGBA(accumulattedColor);
            });
      });
#else
  for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
    for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
      // PIXEL SHADER, but in CPU side
      /* glm::vec4 color = PerPixelSimpleColor(x, y); */
      /* color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f)); */
      /* m_ImageData[x + y * m_FinalImage->GetWidth()] = */
      /*     Utils::ConvertToRGBA(color); */
      m_ImageData[x + y * m_FinalImage->GetWidth()] = 0xffff00ff;
    }
  }
#endif

  m_FinalImage->SetData(m_ImageData);

  if (m_Settings.acumulate)
    m_FrameIndex++;
  else
    m_FrameIndex = 1;
}

glm::vec4 GameRenderer::PerPixelSimpleColor(uint32_t x, uint32_t y) {
  glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
  return glm::vec4{skyColor, 1.0f};
}

// BUG: memory overload
//
glm::vec4 GameRenderer::PerPixel(uint32_t x, uint32_t y) {
  Ray ray;  // origin + direction
  ray.Origin = m_ActiveCamera->GetPosition();
  ray.Direction =
      m_ActiveCamera
          ->GetRayDirections()[x + y * m_FinalImage->GetWidth()];  // with cache

  glm::vec3 light(0.0f);
  glm::vec3 contribution(1.0f);

  int bounces = 5;
  for (int i = 0; i < bounces; i++) {
    // [repeatedly] trace ray with reflection and material
    GameRenderer::HitPayload payload = TraceRay(ray);

    if (payload.HitDistance < 0.0f) {
      glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
      light += skyColor * contribution;
      break;
    }

    // FIXME: how to sample from image
    // vec3 worldPosition;
    // worldPosition.e[0] = payload.WorldPosition.x;
    // worldPosition.e[1] = payload.WorldPosition.y;
    // worldPosition.e[2] = payload.WorldPosition.z;
    // color mapColor = m_EarthTexture->value(payload.u, payload.v,
    // worldPosition ); light.x += (float)mapColor[0]; light.y +=
    // (float)mapColor[1]; light.z += (float)mapColor[2];

    // Get material with roughness and correspondent color
    const Sphere &sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
    const Material &material = m_ActiveScene->Materials[sphere.MaterialIndex];

    // NOTE: absorbing and reflect
    contribution *= material.Albedo;
    // NOTE: adding self emission
    light += material.GetEmission();
    //

    ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
    // NOTE: Simulatting difussion
    ray.Direction =
        glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
  }

  return glm::vec4(light, 1.0f);
}

glm::vec4 GameRenderer::PerPixelSolidColor(uint32_t x, uint32_t y) {
  Ray ray;  // origin + direction
  ray.Origin = m_ActiveCamera->GetPosition();
  ray.Direction =
      m_ActiveCamera
          ->GetRayDirections()[x + y * m_FinalImage->GetWidth()];  // with cache

  glm::vec3 light(0.0f);
  glm::vec3 contribution(1.0f);

  int bounces = 5;
  for (int i = 0; i < bounces; i++) {
    // [repeatedly] trace ray with reflection and material
    GameRenderer::HitPayload payload = TraceRay(ray);
    if (payload.HitDistance < 0.0f) {
      glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
      light += skyColor * contribution;
      break;
    }

    // Get material with roughness and correspondent color
    const Sphere &sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
    const Material &material = m_ActiveScene->Materials[sphere.MaterialIndex];

    contribution *= material.Albedo;
    light += material.GetEmission();

    ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
    ray.Direction =
        glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
  }

  return glm::vec4(light, 1.0f);
}

GameRenderer::HitPayload GameRenderer::TraceRay(const Ray &ray) {
  // (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
  // where
  // a = ray origin
  // b = ray direction
  // r = radius
  // t = hit distance

  int closestSphere = -1;
  float hitDistance = std::numeric_limits<float>::max();
  for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++) {
    const Sphere &sphere = m_ActiveScene->Spheres[i];
    glm::vec3 origin = ray.Origin - sphere.Position;

    float a = glm::dot(ray.Direction, ray.Direction);
    float half_b = glm::dot(origin, ray.Direction);
    float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

    // Quadratic forumula discriminant:
    // b^2 - 4ac

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0.0f)
      continue;

    // Quadratic formula:
    // (-b +- sqrt(discriminant)) / 2a
    // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit
    // distance (currently unused)
    float closestT = -half_b - glm::sqrt(discriminant) / a;
    if (closestT > 0.0f && closestT < hitDistance) {
      hitDistance = closestT;
      closestSphere = (int)i;
    }
  }

  if (closestSphere < 0)
    return Miss(ray);

  return ClosestHit(ray, hitDistance, closestSphere);
}

GameRenderer::HitPayload GameRenderer::ClosestHit(const Ray &ray, float hitDistance,
                                          int objectIndex) {
  GameRenderer::HitPayload payload;
  payload.HitDistance = hitDistance;
  payload.ObjectIndex = objectIndex;

  const Sphere &closestSphere = m_ActiveScene->Spheres[objectIndex];

  // move camera instead to get sphere moved
  glm::vec3 origin = ray.Origin - closestSphere.Position;
  payload.WorldPosition = origin + ray.Direction * hitDistance;
  payload.WorldNormal = glm::normalize(payload.WorldPosition);
  HitPayload::set_sphere_uv(payload.WorldPosition, payload.u, payload.v);
  // move camera back
  payload.WorldPosition += closestSphere.Position;

  return payload;
}

GameRenderer::HitPayload GameRenderer::Miss(const Ray &ray) {
  GameRenderer::HitPayload payload;
  payload.HitDistance = -1.0f;
  return payload;
}
