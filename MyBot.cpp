// (©) Copyright 2019 - Team AASM @ UPB
#include <algorithm>
#include <random>
#include <ctime>

#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"
#include "team_AASM/objective.hpp"
#include "team_AASM/status.hpp"
#include "team_AASM/find_optimal_cell.hpp"

using namespace std;
using namespace hlt;

int main(int argc, char* argv[]) {
    vector<Status> ships_status(1000);
    unsigned int rng_seed;

    if (argc > 1) {
        rng_seed = static_cast<unsigned int> (stoul(argv[1]));
    } else {
        rng_seed = static_cast<unsigned int> (time(nullptr));
    }

    mt19937 rng(rng_seed);
    Game game;

    /**
     * At this point "game" variable is populated with initial map data.
     * This is a good place to do computationally expensive start-up
     * pre-processing.
     * As soon as you call "ready" function below, the 2 second per turn timer
     * will start.
     */
    game.ready("Team_AASM_Bot");

    log::log("Successfully created bot! My Player ID is "
            + to_string(game.my_id) + ". Bot rng seed is "
            + to_string(rng_seed) + ".");

    for (;;) {
        game.update_frame();
        shared_ptr<Player> me = game.me;
        unique_ptr<GameMap>& game_map = game.game_map;

        vector<Command> command_queue;

        for (const auto& ship_iterator : me->ships) {
            /** Commands for ships. */
            shared_ptr<Ship> ship = ship_iterator.second;

            /** Newly created / unloaded ship. */
            if (ships_status[ship->id].objective == Objective::NONE) {
                ships_status[ship->id].objective_pos =
                    optimal_mine_cell(ship, game_map);
                ships_status[ship->id].objective = Objective::NAVIGATE;
            }

            /** Navigate towards designated matrix. */
            if (ships_status[ship->id].objective == Objective::NAVIGATE) {
                if (ships_status[ship->id].objective_pos == ship->position) {
                    /** STOP! Reached destination. */
                    ships_status[ship->id].objective = Objective::MINE;
                    command_queue.push_back(ship->stay_still());
                } else {
                    if (ship->halite < game_map->at(ship->position)->halite
                            / constants::MOVE_COST_RATIO) {
                        command_queue.push_back(ship->stay_still());
                    } else {
                        Direction d = game_map->bruteforce_navigate(ship,
                                    ships_status[ship->id].objective_pos);
                        command_queue.push_back(ship->move(d));
                    }
                }
            } else if (ships_status[ship->id].objective == Objective::MINE) {
                /** Mine until ship is full. */
                if (ship->halite == constants::MAX_HALITE) {
                    /** Full ship. */
                    int min = game_map->height;
                    Position pos;
                    int distance;

                    for (const auto& dropoff : me->dropoffs) {
                        distance =
                            game_map->calculate_distance(ship->position,
                            dropoff.second->position);
                        if (distance < min) {
                            min = distance;
                            pos = dropoff.second->position;
                        }
                    }

                    distance = game_map->calculate_distance(ship->position,
                            me->shipyard->position);

                    if (distance < min) {
                        min = distance;
                        pos = me->shipyard->position;
                    }

                    game_map->at(ship->position)->marked_as_target = false;
                    ships_status[ship->id].objective = Objective::UNLOAD;
                    ships_status[ship->id].objective_pos = pos;
                } else if (game_map->at(ship)->halite
                        > constants::HALITE_WORTH_MINING
                        || (ship->halite < game_map->at(ship)->halite
                            / constants::MOVE_COST_RATIO)) {
                    /** We should continue mining in this cell. */
                    command_queue.push_back(ship->stay_still());
                } else {
                    /** Not enough halite in this cell. */
                    game_map->at(ship->position)->marked_as_target = false;
                    ships_status[ship->id].objective = Objective::NAVIGATE;
                    ships_status[ship->id].objective_pos =
                        optimal_mine_cell(ship, game_map);
                    Direction d = game_map->bruteforce_navigate(ship,
                                        ships_status[ship->id].objective_pos);
                    command_queue.push_back(ship->move(d));
                }
            }

            if (ships_status[ship->id].objective == Objective::UNLOAD) {
                if (ships_status[ship->id].objective_pos == ship->position) {
                    ships_status[ship->id].objective = Objective::NAVIGATE;
                    ships_status[ship->id].objective_pos =
                        optimal_mine_cell(ship, game_map);
                }

                if ((ship->halite != constants::MAX_HALITE &&
                        game_map->at(ship->position)->halite >
                            constants::HALITE_WORTH_MINING)
                        || (ship->halite < game_map->at(ship)->halite
                            / constants::MOVE_COST_RATIO)) {
                    command_queue.push_back(ship->stay_still());
                } else {
                    Direction d;
                    if (game_map->at(ships_status[ship->id].
                                objective_pos)->is_occupied()
                            && game_map->at(ships_status[ship->id].
                                objective_pos)->ship->owner != me->id
                            && game_map->calculate_distance(ship->position,
                            ships_status[ship->id].objective_pos) == 1) {
                        d = game_map->kamikaze_navigate(ship,
                                ships_status[ship->id].objective_pos);
                    } else {
                        d = game_map->bruteforce_navigate(ship,
                                ships_status[ship->id].objective_pos);
                    }
                    command_queue.push_back(ship->move(d));
                }
            }
        }

        if (game.turn_number <= 200 && me->halite >= constants::SHIP_COST &&
                !game_map->at(me->shipyard)->is_occupied()) {
            /** Ship creation. */
            command_queue.push_back(me->shipyard->spawn());
        }

        if (!game.end_turn(command_queue)) {
            break;
        }
    }

    return 0;
}
