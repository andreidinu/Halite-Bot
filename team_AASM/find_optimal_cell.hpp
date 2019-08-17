#pragma once

#include "hlt/game.hpp"

using namespace std;
using namespace hlt;

/** Greedy search of optimal cell. */
Position optimal_mine_cell(shared_ptr<Ship> ship,
                           unique_ptr<GameMap>& game_map) {
    Position optimal;
    int max = 0;

    for (int y = 0; y < game_map->height; ++y) {
        for (int x = 0; x < game_map->width; ++x) {
            /**
             * Choose a cell as target as long as it is not targeted by other
             * ship.
             */
            if (!game_map->cells[y][x].marked_as_target) {
                Position checked = {x, y};
                int distance =
                    game_map->calculate_distance(ship->position, checked);

                if (distance) {
                    int ratio = game_map->cells[y][x].halite / distance;

                    if (ratio > max) {
                        max = ratio;
                        optimal = checked;
                    }
                }
            }
        }
    }

    /** Mark cell as targeted. */
    game_map->cells[optimal.y][optimal.x].marked_as_target = true;

    return optimal;
}
