#pragma once

#include "types.hpp"
#include "map_cell.hpp"

#include <vector>

namespace hlt {
    struct GameMap {
        int width;
        int height;
        std::vector<std::vector<MapCell>> cells;

        MapCell* at(const Position& position) {
            Position normalized = normalize(position);
            return &cells[normalized.y][normalized.x];
        }

        MapCell* at(const Entity& entity) {
            return at(entity.position);
        }

        MapCell* at(const Entity* entity) {
            return at(entity->position);
        }

        MapCell* at(const std::shared_ptr<Entity>& entity) {
            return at(entity->position);
        }

        int calculate_distance(const Position& source, const Position& target) {
            const auto& normalized_source = normalize(source);
            const auto& normalized_target = normalize(target);

            const int dx = std::abs(normalized_source.x - normalized_target.x);
            const int dy = std::abs(normalized_source.y - normalized_target.y);

            const int toroidal_dx = std::min(dx, width - dx);
            const int toroidal_dy = std::min(dy, height - dy);

            return toroidal_dx + toroidal_dy;
        }

        Position normalize(const Position& position) {
            const int x = ((position.x % width) + width) % width;
            const int y = ((position.y % height) + height) % height;
            return { x, y };
        }

        std::vector<Direction> get_unsafe_moves(const Position& source, const Position& destination) {
            const auto& normalized_source = normalize(source);
            const auto& normalized_destination = normalize(destination);

            const int dx = std::abs(normalized_source.x - normalized_destination.x);
            const int dy = std::abs(normalized_source.y - normalized_destination.y);
            const int wrapped_dx = width - dx;
            const int wrapped_dy = height - dy;

            std::vector<Direction> possible_moves;

            if (normalized_source.x < normalized_destination.x) {
                possible_moves.push_back(dx > wrapped_dx ? Direction::WEST : Direction::EAST);
            } else if (normalized_source.x > normalized_destination.x) {
                possible_moves.push_back(dx < wrapped_dx ? Direction::WEST : Direction::EAST);
            }

            if (normalized_source.y < normalized_destination.y) {
                possible_moves.push_back(dy > wrapped_dy ? Direction::NORTH : Direction::SOUTH);
            } else if (normalized_source.y > normalized_destination.y) {
                possible_moves.push_back(dy < wrapped_dy ? Direction::NORTH : Direction::SOUTH);
            }
            
            if (possible_moves.size() == 2 && 
                    std::min(dy, wrapped_dy) > std::min(dx, wrapped_dx)) {
                auto aux = possible_moves[0];
                possible_moves[0] = possible_moves[1];
                possible_moves[1] = aux;
            }

            return possible_moves;
        }

        Direction naive_navigate(std::shared_ptr<Ship> ship, const Position& destination) {
            // get_unsafe_moves normalizes for us
            for (const auto &direction : get_unsafe_moves(ship->position, destination)) {
                Position target_pos = ship->position.directional_offset(direction);
                if (!at(target_pos)->is_occupied()) {
                    at(target_pos)->mark_unsafe(ship);
                    return direction;
                }
            }

            return Direction::STILL;
        }
        
        Direction bruteforce_navigate(std::shared_ptr<Ship> ship,
                                      const Position& destination) {
            std::vector<Direction> directions = get_unsafe_moves(ship->position,
                                                                 destination);
            if (directions.size() == 2) {
                directions.push_back(invert_direction(directions[1]));
                directions.push_back(invert_direction(directions[0]));
            } else if (directions.size() == 1) {
                if (directions[0] == Direction::NORTH
                        || directions[0] == Direction::SOUTH) {
                    directions.push_back(Direction::EAST);
                    directions.push_back(Direction::WEST);
                } else {
                    directions.push_back(Direction::NORTH);
                    directions.push_back(Direction::SOUTH);
                }
                directions.push_back(invert_direction(directions[0]));
            }
            
            for (const auto &direction : directions) {
                Position target_pos = ship->position.directional_offset(direction);
                if (!at(target_pos)->is_occupied()) {
                    at(target_pos)->mark_unsafe(ship);
                    return direction;
                }
            }

            return Direction::STILL;
        }
        
        Direction kamikaze_navigate(std::shared_ptr<Ship> ship,
                                      const Position& destination) {
            const auto& moves = get_unsafe_moves(ship->position, destination);
            if (!moves.empty()) {
                Position t_pos = ship->position.directional_offset(moves[0]);
                at(t_pos)->mark_unsafe(ship);
                return moves[0];
            }
            return Direction::STILL;
        }

        void _update();
        static std::unique_ptr<GameMap> _generate();
    };
}
