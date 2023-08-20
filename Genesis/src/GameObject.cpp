#pragma once
#include "GameObject.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <utility>

GameObject::~GameObject(){};

GameObject::GameObject(std::shared_ptr<SpriteSheet> spritSheet,
                       std::shared_ptr<Transform2D> transform)
    : m_SpritSheet(std::move(spritSheet)), m_Transform(transform){};

void GameObject::Draw(std::shared_ptr<GameRenderer> renderer) {
  // uint32_t* pixels = renderer->GetImageData();

  // glm::mat4 model = glm::mat4(1.0);
  // model = glm::translate(model, glm::vec3(m_Coodx, m_Coody, 0.0));
  // model = glm::rotate(model, glm::radians(m_Transform->Rotation),
  //                     glm::vec3(0.0f, 0.0f, 1.0f));
  // model = glm::scale(model, glm::vec3(m_Transform->Size, 1.0f));

  //   for (uint32_t y = 0; y < renderer->GetHeight(); y++) {
  //     for (uint32_t x = 0; x < renderer->GetWidth(); x++)
  //       pixels[x + y * renderer->GetWidth()] = pixels[x + y *
  //       renderer->GetWidth()];
  //   }

  renderer->RenderSprite(m_Coodx, m_Coody);
};

void GameObject::Update(std::shared_ptr<GameRenderer> renderer, float ts) {

  float offset_x = m_Transform->Velocity.x * ts * 100;
  float offset_y = m_Transform->Velocity.y * ts * 100;

  renderer->xt += offset_x;
  renderer->yt += offset_y;
  
  std::cout << renderer->xt << std::endl;

  auto spriteSize = renderer->spriteSize;
  // FIXME: must check window size fo ot crash
  if (renderer->xt > 800)
    renderer->xt = 800;
  if (renderer->xt < 0)
    renderer->xt = 0;

  if (renderer->yt > 600)
    renderer->yt = 600;
  if (renderer->yt < 0)
    renderer->yt = 0;

  renderer->Update(ts);
};