#include "balls.hpp"

Ball::Ball(const sf::Vector2f& spd, sf::Sprite sprite) : speed(std::move(spd)), sprite(sprite) {}

void Ball::updatePosition(float dt) {
  sf::Vector2f delta_position{speed.x * dt, speed.y * dt};
  sprite.move(delta_position);
}
