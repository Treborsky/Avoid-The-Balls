/*
 * rng.hpp
 *
 *  Created on: 11 paź 2022
 *      Author: robko
 */

#ifndef INCLUDE_RNG_HPP_
#define INCLUDE_RNG_HPP_

#include <random>

class Rng {
 public:
  Rng(float min, float max);
  ~Rng() = default;

  float generate();

 private:
  std::mt19937 _gen;
  std::uniform_real_distribution<float> _distr;
};


#endif /* INCLUDE_RNG_HPP_ */
