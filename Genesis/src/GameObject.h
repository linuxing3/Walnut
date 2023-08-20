#pragma once
#include "GameRenderer.h"
#include "SpriteSheet.h"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <memory>

class GameObject {
 public:
  struct Transform2D {
   public:
    glm::vec2 Postion = {0.0f, 0.0f};
    glm::vec2 Size = {0.0f, 0.0f};
    glm::vec2 Velocity = {2.5f, 0.0f};
    glm::vec3 Color = {0.0f, 0.0f, 0.0f};
    float Rotation = 0.25f;
    
    Transform2D() = default;

    glm::mat4x4 mvp(glm::mat4x4 m, glm::mat4x4 v, glm::mat4x4 p) {
      return p * v * m;
    }

    glm::vec2 SetPostion(float x, float y) {
      Postion.x = x;
      Postion.y = y;
    };

    glm::vec2 SetColor(float r, float g, float b) {
      Color.r = r;
      Color.g = g;
      Color.b = b;
    };
  };

  GameObject(std::shared_ptr<SpriteSheet> spritSheet,
             std::shared_ptr<Transform2D> transform);
  ~GameObject();
  void Draw(std::shared_ptr<GameRenderer> renderer);

  bool m_IsSolid;
  bool m_Destroyed;
  std::shared_ptr<SpriteSheet> getSpritSheet() { return m_SpritSheet; };
  int GetCoodx() { return m_Coodx; };
  int GetCoody() { return m_Coody; };
  void Update(std::shared_ptr<GameRenderer> renderer, float ts);

 private:
  std::shared_ptr<SpriteSheet> m_SpritSheet;
  std::shared_ptr<Transform2D> m_Transform;
  int m_Coodx = 4, m_Coody = 1;
};