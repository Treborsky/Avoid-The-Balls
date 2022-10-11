/*
 * rng.cpp
 *
 *  Created on: 11 paź 2022
 *      Author: robko
 */

#include "rng.hpp"

Rng::Rng(float min=0.0f, float max=1.0f) : _distr{min, max} {
  std::random_device rd;
  _gen = std::mt19937(rd());
}

float Rng::generate(){
  return _distr(_gen);
}
