#pragma once
#include "GameObject.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <utility>

GameObject::~GameObject(){};

GameObject::GameObject(std::shared_ptr<SpriteSheet> spritSheet,
                       std::shared_ptr<Transform2D> transform)
    : m_SpritSheet(std::move(spritSheet)), m_Transform(transform){};

void GameObject::Draw(std::shared_ptr<GameRenderer> renderer) {
  uint32_t* pixels = renderer->GetImageData();

  glm::mat4 model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(m_Coodx, m_Coody, 0.0));
  model = glm::rotate(model, glm::radians(m_Transform->Rotation),
                      glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, glm::vec3(m_Transform->Size, 1.0f));

//   for (uint32_t y = 0; y < renderer->GetHeight(); y++) {
//     for (uint32_t x = 0; x < renderer->GetWidth(); x++)
//       pixels[x + y * renderer->GetWidth()] = pixels[x + y * renderer->GetWidth()];
//   }

  renderer->RenderSprite(m_Coodx, m_Coody);
};

void GameObject::Update(std::shared_ptr<GameRenderer> renderer, float ts) {

  float offset_x = m_Transform->Velocity.x * ts * 1000 * 1000;
  float offset_y = m_Transform->Velocity.x * ts * 1000 * 1000;
  
  // m_Transform->SetPostion(0.0, 0.0);
  // if (m_Transform->Postion.x < 0) {
  //   m_Transform->Postion.x = 0;
  //   m_Transform->Postion.x = 0;
  // }
  // if (m_Transform->Postion.x > 800) {
  //   m_Transform->Postion.x = 800;
  // }

  // if (m_Transform->Postion.y < 0) {
  //   m_Transform->Postion.y = 0;
  // }
  // if (m_Transform->Postion.y > 600) {
  //   m_Transform->Postion.y = 600;
  // }

  // renderer->SetStartPosition(m_Transform->Postion.x, m_Transform->Postion.y);
  
  renderer->xt += offset_x;
  renderer->yt = 500;

  if (renderer->xt > 800)
    renderer->xt -= offset_x;
  if (renderer->xt < 0)
    renderer->xt += offset_x;

  renderer->Update(ts);
};