#include "command.hpp"

#include <iostream>

constexpr char GENERATE = 'g';
constexpr char CONSTRUCT[] = "c ";
constexpr char MOVE[] = "m ";

hlt::Command hlt::command::spawn_ship() {
    /** "g" */
    return std::string(1, GENERATE);
}

hlt::Command hlt::command::transform_ship_into_dropoff_site(EntityId id) {
    /** "c id_nava_transformata" */
    return CONSTRUCT + std::to_string(id);
}

/**
 * Available directions:
 *   NORTH = 'n',
 *   EAST = 'e',
 *   SOUTH = 's',
 *   WEST = 'w',
 *   STILL = 'o'
 */
hlt::Command hlt::command::move(EntityId id, hlt::Direction direction) {
    /** "m id_nava_mutata directie" */
    return MOVE + std::to_string(id) + ' ' + static_cast<char>(direction);
}
