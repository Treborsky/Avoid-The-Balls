#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "rng.hpp"
#include "balls.hpp"

#define LOG(s) std::cout << "[DEBUG] " << s << std::endl

namespace {
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
  constexpr const sf::Int64 newBallSpawnCountdown = static_cast<sf::Int64>(.2 * ONE_SEC_IN_US); // [us] (1 second)
  constexpr const sf::Int64 framePeriod = 16666; // [us]
}

int main() {

  sf::Font font;
  sf::Image player_image;
  sf::Image ball_image;
  sf::Texture player_texture;
  sf::Texture ballTexture;

  {
    if (!font.loadFromFile("C:\\proj\\eclipse_sfml_test\\sfml_test\\assets\\Roboto-Regular.ttf")){
      LOG("Error when loading Roboto font");
      return EXIT_FAILURE;
    }

    if (!player_image.loadFromFile("C:\\proj\\eclipse_sfml_test\\sfml_test\\assets\\player.png")) {
      LOG("Error when loading player image file");
      return EXIT_FAILURE;
    }

    if (!ball_image.loadFromFile("C:\\proj\\eclipse_sfml_test\\sfml_test\\assets\\ball.png")) {
      LOG("Error when loading ball image file");
      return EXIT_FAILURE;
    }

    if (!player_texture.loadFromImage(player_image)){
      LOG("Error when loading player texture");
      return EXIT_FAILURE;
    }

    if (!ballTexture.loadFromImage(ball_image)){
      LOG("Error when loading player texture");
      return EXIT_FAILURE;
    }

    LOG("Loaded textures");
  }

  Rng speedRng(BALL_SPEED_MIN, BALL_SPEED_MAX);
  Rng positionRng(static_cast<float>(BALL_SPRITE_W), static_cast<float>(WINDOW_W - BALL_SPRITE_W));

  LOG("RNG initialized");

	sf::RenderWindow gameWindow;
	gameWindow.create(sf::VideoMode(WINDOW_W, WINDOW_H), "SFML playaround");
	if (!gameWindow.isOpen()){
	  LOG("Error when creating the window, exiting");
	  return EXIT_FAILURE;
	}

	sf::Text fpsText;
	sf::Text tpfText;
	sf::Text ballCounterText;

	{
    fpsText.setFont(font);
    fpsText.setCharacterSize(12);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setStyle(sf::Text::Regular);

    tpfText.setFont(font);
    tpfText.setCharacterSize(12);
    tpfText.setFillColor(sf::Color::White);
    tpfText.setStyle(sf::Text::Regular);
    tpfText.move(0.0f, 15.0f);

    ballCounterText.setFont(font);
    ballCounterText.setCharacterSize(12);
    ballCounterText.setFillColor(sf::Color::White);
    ballCounterText.setStyle(sf::Text::Regular);
    ballCounterText.move(sf::Vector2f{WINDOW_W - 100.f, 0.f});
	}

  sf::Sprite player_sprite;
  {
    player_sprite.setTexture(player_texture);
    player_sprite.setScale(PLAYER_SPRITE_SCALEX, PLAYER_SPRITE_SCALEY);
    player_sprite.setPosition(static_cast<float>(WINDOW_W + PLAYER_SPRITE_SCALED_W)/2, static_cast<float>(WINDOW_H - PLAYER_SPRITE_SCALED_H));
  }

  auto getBallCountString = [&](unsigned int ballcount){
      std::stringstream ss;
      ss << "ball count  " << ballcount;
      return ss.str();
    };

	auto getFramerateString = [&](double framerate){
	  std::stringstream ss;
	  ss << "fps: " << framerate;
	  return ss.str();
	};

	auto getTimePerFrameString = [&](double time){
    std::stringstream ss;
    ss << "tpf: " << time;
    return ss.str();
  };

	sf::Clock tickClock;
  sf::Clock ballClock;
  sf::Clock frameClock;
  std::vector<Ball> balls;
  sf::Time ballDt;
  double framerate = 0;

	// main game loop
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
      Ball newBall(sf::Vector2f{0.f, speedRng.generate()}, sf::Sprite(ballTexture));
      newBall.sprite.setPosition(sf::Vector2f{positionRng.generate(), 0.f});
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
    gameWindow.draw(tpfText);
    gameWindow.draw(ballCounterText);
    for(const auto& ball : balls) {
      gameWindow.draw(ball.sprite);
    }
    gameWindow.draw(player_sprite);
    gameWindow.display();

    tpfText.setString(getTimePerFrameString(static_cast<double>(frameClock.getElapsedTime().asMicroseconds() - beginLoopTime)));
    while (frameClock.getElapsedTime().asMicroseconds() - beginLoopTime < framePeriod) {}
    const auto endLoopTime = frameClock.getElapsedTime().asMicroseconds();
    framerate = ONE_SEC_IN_US / static_cast<double>(endLoopTime - beginLoopTime);
    fpsText.setString(getFramerateString(framerate));

    frameClock.restart();
	}

	return EXIT_SUCCESS;
}
