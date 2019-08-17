#pragma once

enum class Objective {
    /** Place holder, indicates that the ship has just spawned */
    NONE,
    /** The ship is mining */
    MINE,
    /** The ship is heading towards a mining spot or dropoff point to unload */
    NAVIGATE,
    /** The map cell must be cleared ASAP */
    RUN,
    /** The ship is heading towards a cell to become a dropoff point */
    CREATE,
    /** The ship must unload halite. */
    UNLOAD,
};
