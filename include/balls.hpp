#ifndef SFML_TEST_BALLS_HPP_
#define SFML_TEST_BALLS_HPP_

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class Ball {
public:
  Ball(const sf::Vector2f& spd, sf::Sprite sprite);
  ~Ball() = default;
  void updatePosition(float dt);
  sf::Vector2f speed;
  sf::Sprite sprite;
};

#endif // SFML_TEST_BALLS_HPP_
