#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <random>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "balls.hpp"

#define LOG(s) std::cout << "[DEBUG] " << s << std::endl

int main() {

  sf::Font roboto_font;
  if (!roboto_font.loadFromFile("C:\\proj\\eclipse_sfml_test\\sfml_test\\assets\\Roboto-Regular.ttf")){
    LOG("Error when loading Roboto font");
    return EXIT_FAILURE;
  }

  sf::Image player_image;
  if (!player_image.loadFromFile("C:\\proj\\eclipse_sfml_test\\sfml_test\\assets\\player.png")) {
    LOG("Error when loading player image file");
  }

  sf::Image ball_image;
  if (!ball_image.loadFromFile("C:\\proj\\eclipse_sfml_test\\sfml_test\\assets\\ball.png")) {
    LOG("Error when loading ball image file");
  }

  sf::Texture player_texture;
  if (!player_texture.loadFromImage(player_image)){
    LOG("Error when loading player texture");
  }

  sf::Texture ballTexture;
  if (!ballTexture.loadFromImage(ball_image)){
    LOG("Error when loading player texture");
  }

  LOG("Loading textures complete");

  constexpr const unsigned int WINDOW_W = 800U;
  constexpr const unsigned int WINDOW_H = 600U;
  constexpr const float PLAYER_SPRITE_SCALEX = 2.f;
  constexpr const float PLAYER_SPRITE_SCALEY = 2.f;
  constexpr const unsigned int PLAYER_SPRITE_W = 20U;
  constexpr const unsigned int PLAYER_SPRITE_SCALED_W = PLAYER_SPRITE_W * static_cast<unsigned int>(PLAYER_SPRITE_SCALEX);
  constexpr const unsigned int PLAYER_SPRITE_H = 35U;
  constexpr const unsigned int PLAYER_SPRITE_SCALED_H = PLAYER_SPRITE_H * static_cast<unsigned int>(PLAYER_SPRITE_SCALEY);
  constexpr const unsigned int BALL_SPRITE_W = 20U;
  constexpr const float BALL_SPEED_MIN = 15.f;
  constexpr const float BALL_SPEED_MAX = 20.f;
  constexpr const double ONE_SEC_IN_US = 1.e6;

  LOG("Game constants intialized");

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> speedDistr(BALL_SPEED_MIN, BALL_SPEED_MAX);
  auto getRandomBallSpawnSpeed = [&]() {
      return speedDistr(gen);
  };
  std::uniform_real_distribution<float> positionDistr(static_cast<float>(BALL_SPRITE_W), static_cast<float>(WINDOW_W - BALL_SPRITE_W));
  auto getRandomBallSpawnPosition = [&]() {
    return positionDistr(gen);
  };

  LOG("RNG initialized");

	sf::RenderWindow gameWindow;
	gameWindow.create(sf::VideoMode(WINDOW_W, WINDOW_H), "SFML playaround");
	if (!gameWindow.isOpen()){
	  LOG("Error when creating the window, exiting");
	  return EXIT_FAILURE;
	}

	sf::Text fpsText;
	fpsText.setFont(roboto_font);
	fpsText.setCharacterSize(12);
	fpsText.setFillColor(sf::Color::White);
	fpsText.setStyle(sf::Text::Regular);

	sf::Text ballCounterText;
	ballCounterText.setFont(roboto_font);
	ballCounterText.setCharacterSize(12);
	ballCounterText.setFillColor(sf::Color::White);
	ballCounterText.setStyle(sf::Text::Regular);
	ballCounterText.move(sf::Vector2f{WINDOW_W - 100.f, 0.f});

  sf::Sprite player_sprite;
  player_sprite.setTexture(player_texture);
  player_sprite.setScale(PLAYER_SPRITE_SCALEX, PLAYER_SPRITE_SCALEY);
  player_sprite.setPosition(static_cast<float>(WINDOW_W + PLAYER_SPRITE_SCALED_W)/2, static_cast<float>(WINDOW_H - PLAYER_SPRITE_SCALED_H));

  sf::Clock tickClock;

  sf::Clock ballClock;
  std::vector<Ball> balls;
  sf::Time ballDt;

  auto getBallCountString = [&](unsigned int ballcount){
      std::stringstream ss;
      ss << "ball count  " << ballcount;
      return ss.str();
    };

	sf::Clock frameClock;
	double framerate = 0;

	auto getFramerateString = [&](double framerate){
	  std::stringstream ss;
	  ss << "fps " << framerate;
	  return ss.str();
	};

	constexpr const sf::Int64 newBallSpawnCountdown = static_cast<sf::Int64>(.2 * ONE_SEC_IN_US); // [us] (1 second)
	constexpr const sf::Int64 framePeriod = 16666; // [us]

	sf::Event evt;
	while (gameWindow.isOpen()) {
    const auto beginLoopTime = frameClock.getElapsedTime().asMicroseconds();

    while (gameWindow.pollEvent(evt)) {
      switch (evt.type) {
        case sf::Event::Closed: {
          LOG("closing the window");
          gameWindow.close();
          break;
        }
        default: {
          break;
        }
      }
    }

    if (gameWindow.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player_sprite.move(-5.0f, 0.0f);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player_sprite.move(5.0f, 0.0f);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        LOG("closing the window");
        gameWindow.close();
      }
    }

    if (newBallSpawnCountdown - ballClock.getElapsedTime().asMicroseconds() < 0) {
      Ball newBall(sf::Vector2f{0.f, getRandomBallSpawnSpeed()}, sf::Sprite(ballTexture));
      newBall.sprite.setPosition(sf::Vector2f{getRandomBallSpawnPosition(), 0.f});
      balls.emplace_back(newBall);
      ballClock.restart();
    }

    ballDt = tickClock.getElapsedTime() - ballDt;

    std::for_each(balls.begin(), balls.end(), [](auto& ball){
      ball.updatePosition(0.25f);
    });

    balls.erase(std::remove_if(balls.begin(), balls.end(), [&](auto& ball){
      return ball.sprite.getPosition().y > WINDOW_H;
    }), balls.end());

    ballCounterText.setString(getBallCountString(static_cast<unsigned int>(balls.size())));

    gameWindow.clear(sf::Color::Black);
    gameWindow.draw(fpsText);
    gameWindow.draw(ballCounterText);
    for(const auto& ball : balls) {
      gameWindow.draw(ball.sprite);
    }
    gameWindow.draw(player_sprite);
    gameWindow.display();

    while (frameClock.getElapsedTime().asMicroseconds() - beginLoopTime < framePeriod) {}
    const auto endLoopTime = frameClock.getElapsedTime().asMicroseconds();
    framerate = ONE_SEC_IN_US / static_cast<double>(endLoopTime - beginLoopTime);
    fpsText.setString(getFramerateString(framerate));

    frameClock.restart();
	}

	return EXIT_SUCCESS;
}
