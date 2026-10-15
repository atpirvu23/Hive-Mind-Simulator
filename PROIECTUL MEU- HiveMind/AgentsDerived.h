#pragma once

#include "Utils.h"
#include "Agent.h"

using namespace std;

class Drone : public Agent
{

public:
    Drone() : Agent(100, 3, 10, 15, 1) {}

    string getType() const override
    {
        return "Drona";
    }
    bool canPassWall() const override
    {
        return true; // numai drona poate "zbura" peste pereti
    }
};

class Robot : public Agent
{
public:
    Robot() : Agent(300, 1, 2, 1, 4) {}
    string getType() const override
    {
        return "Robot";
    }
    bool canPassWall() const override
    {
        return false;
    }
};

class Scooter : public Agent
{
public:
    Scooter() : Agent(200, 2, 5, 4, 2) {}
    string getType() const override
    {
        return "Scooter";
    }
    bool canPassWall() const override
    {
        return false;
    }
};