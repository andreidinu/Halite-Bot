#pragma once

#include "types.hpp"
#include "position.hpp"
#include "ship.hpp"
#include "dropoff.hpp"

namespace hlt {
    struct MapCell {
        Position position;
        Halite halite;
        std::shared_ptr<Ship> ship;
        bool marked_as_target = false;

        /** 
         * only has dropoffs and shipyards; if id is -1, then it's a shipyard,
         * otherwise it's a dropoff
         */
        std::shared_ptr<Entity> structure;

        MapCell(const int &x, const int &y, const Halite &halite) :
            position(x, y),
            halite(halite)
        {}

        /**
         * Checks if a map_cell is empty or not a.k.a has a ship or
         * structure.
         */
        bool is_empty() const {
            return !ship && !structure;
        }

        /** Checks if there is any ship on this cell. */
        bool is_occupied() const {
            return static_cast<bool>(ship);
        }

        /** Checks if there is any structure on this cell. */
        bool has_structure() const {
            return static_cast<bool>(structure);
        }

        /** Marks this cell as occupied by the parameter ship. */
        void mark_unsafe(std::shared_ptr<Ship>& ship) {
            this->ship = ship;
        }
    };
}
