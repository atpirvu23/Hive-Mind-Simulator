// acest fisier este un header comun care defineste tipuri de date folosite in proiect

#pragma once

// structura pozitiilor pe harta
struct Position
{
    int r;
    int c;

    Position() : r(0), c(0) {}
    Position(int r, int c) : r(r), c(c) {}

    bool operator==(const Position &other) const
    {
        return r == other.r && c == other.c;
    }
};

// tipuri de celule din harta

enum CellType
{
    EMPTY,   //.
    WALL,    // #
    HUB,     // B
    STATION, // S
    CLIENT   // D
};

// tipuri de agenti, curieri
enum AgentType
{
    DRONE,
    ROBOT,
    SCOOTER
};

// tipuri de stare a unui curier
enum AgentState
{
    IDLE,
    MOVING,
    CHARGING,
    DEAD
};