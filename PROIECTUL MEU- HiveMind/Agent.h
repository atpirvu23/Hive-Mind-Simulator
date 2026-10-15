#pragma once

#include "Utils.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Agent
{
protected:
    Position position;
    int batteryLevel;
    int maxBattery;
    int speed;
    int consumption;
    int costPerTick;
    int capacity;
    AgentState state;

    std::vector<Position> currentPath; // pasii de urmat
    int currentPackageId;

public:
    Agent(int maxBat, int spd, int cons, int cost, int cap) : maxBattery(maxBat), speed(spd), consumption(cons), costPerTick(cost), capacity(cap)
    {

        batteryLevel = maxBattery;
        state = IDLE;
        position = {0, 0};
        currentPackageId = -1;
    }

    virtual ~Agent() {}

    bool isAlive() const
    {
        return state != DEAD;
    }

    AgentState getState() const
    {
        return state;
    }

    Position getPosition() const
    {
        return position;
    }

    int getCapacity() const
    {
        return capacity;
    }

    virtual string getType() const = 0;
    virtual bool canPassWall() const = 0; // doar dronele pot trece zidurile

    void consumeBattery()
    {
        batteryLevel = batteryLevel - consumption;
        if (batteryLevel <= 0)
        {
            batteryLevel = 0;
            state = DEAD;
        }
    }

    void setPosition(int r, int c)
    {
        position = {r, c};
    }

    void setPath(const std::vector<Position> &path)
    {
        currentPath = path;
    }

    bool hasPath() const
    {
        return !currentPath.empty();
    }

    // facem o functie care ne zice urmamtoarea pozitie si o sterge din lista
    Position moveNext()
    {
        if (currentPath.empty()) {
            return position;
        }
        Position next = currentPath.front();
        currentPath.erase(currentPath.begin());
        return next;
    }

    void assignPackage(int pkgId) { currentPackageId = pkgId; }
    int getPackageId() const { return currentPackageId; }
    void clearPackage() { currentPackageId = -1; }

    void setState(AgentState s) { state = s; }

    // incarcare bateria (+25% din max)
    void charge()
    {
        int chargeAmount = maxBattery / 4;
        batteryLevel = batteryLevel + chargeAmount;
        if (batteryLevel > maxBattery) {
            batteryLevel = maxBattery;
        }
    }

    // returneaza true daca bateria e la < 20%
    bool isLowBattery() const
    {
        return batteryLevel < (maxBattery * 0.2);
    }

    int getCostPerTick() const
    {
        return costPerTick;
    }

    int getBatteryLevel() const
    {
        return batteryLevel;
    }

    int getMaxBattery() const
    {
        return maxBattery;
    }
};