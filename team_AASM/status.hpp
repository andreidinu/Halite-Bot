#pragma once

#include "hlt/game.hpp"

using namespace std;
using namespace hlt;

typedef struct {
    Position objective_pos;
    Objective objective = Objective::NONE;
} Status;