#pragma once
#include "GameObject.h"
#include "Walnut/Random.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <utility>

GameObject::~GameObject(){};

GameObject::GameObject(std::shared_ptr<SpriteSheet> spritSheet,
                       std::shared_ptr<Transform2D> transform)
    : m_SpritSheet(std::move(spritSheet)), m_Transform(transform){};

void GameObject::Draw(std::shared_ptr<GameRenderer> renderer) {

#ifdef MVP
  uint32_t w, h;
  void* data = renderer->GetFinalImage()->Decode(
      renderer->GetImageData(), sizeof(renderer->GetImageData()), w, h);

  glm::mat4 model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(m_Coodx, m_Coody, 0.0));
  model = glm::rotate(model, glm::radians(m_Transform->Rotation),
                      glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, glm::vec3(m_Transform->Size, 1.0f));
#endif
  // change sprite
  m_Coodx = Walnut::Random::UInt(0, 7);
  renderer->RenderSprite(m_Coodx, m_Coody);
};

void GameObject::Update(std::shared_ptr<GameRenderer> renderer, float ts) {

  auto width = renderer->m_WindowWidth;
  auto height = renderer->m_WindowHeight;
  auto spriteSize = renderer->spriteSize * 2;

  // move sprite
  float offset_x = m_Transform->Velocity.x * ts * 1000;
  float offset_y = m_Transform->Velocity.y * ts * 1000;

  if (left) {
    renderer->xt += offset_x;
  } else {
    renderer->xt -= offset_x;
  }
  if (up) {
    renderer->yt += offset_y;
  } else {
    renderer->yt -= offset_y;
  }

  if (renderer->xt > width - spriteSize) {
    left = false;
  };
  if (renderer->xt < spriteSize) {
    left = true;
  }
  if (renderer->yt > height - spriteSize) {
    up = false;
  }
  if (renderer->yt < spriteSize) {
    up = true;
  }

  renderer->Update(ts);
};
