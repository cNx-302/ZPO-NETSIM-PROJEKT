//
// Created by Błażej on 19.01.2026.
//

#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <functional>
#include <random>

#include "types.hpp"

extern std::random_device rd;
extern std::mt19937 rng;

extern double default_probability_generator();

extern ProbabilityGenerator probability_generator;

#endif //HELPERS_HPP
