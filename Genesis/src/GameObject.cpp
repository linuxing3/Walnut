#pragma once
#include "GameObject.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
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
  renderer->RenderSprite(m_Coodx, m_Coody);
};

void GameObject::Update(std::shared_ptr<GameRenderer> renderer, float ts) {

  // change sprite
  {
    m_Coodx += 1;
    if (m_Coodx > 8) {
      m_Coodx = 0;
    }
    sleep(ts * 10);
  }

  // move sprite
  float offset_x = m_Transform->Velocity.x * ts * 100;
  float offset_y = m_Transform->Velocity.y * ts * 100;

  renderer->xt += offset_x;
  renderer->yt += offset_y;

  std::cout << renderer->xt << std::endl;

  auto width = renderer->m_WindowWidth;
  auto height = renderer->m_WindowHeight;
  auto spriteSize = renderer->spriteSize;

  // FIXME: must check window size fo ot crash
  if (renderer->xt > width - spriteSize)
    renderer->xt -= offset_x;
  if (renderer->xt < spriteSize)
    renderer->xt += offset_x;

  if (renderer->yt > height - spriteSize)
    renderer->yt -= offset_y;
  if (renderer->yt < spriteSize)
    renderer->yt += offset_y;

  renderer->Update(ts);
};