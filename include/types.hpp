//
// Created by Błażej on 19.01.2026.
//

#ifndef TYPES_HPP
#define TYPES_HPP

#include <functional>

using ElementID = int;
static const ElementID EMPTY_ID = -1;
using Time = int;
using TimeOffset = int;
using ProbabilityGenerator = std::function<double()>;

#endif //TYPES_HPP
