#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "Utils.h"
#include "Agent.h"
#include "Package.h"

using namespace std; /* este cam inutil aici si imi dadea
 multe erori in fisierele de tip .h unde introduceam using namespace std;*/

// Singleton- implementam design pattern
class SimulationManager
{
private:
    SimulationManager();

    int mapWidth;
    int mapHeight;
    int maxTicks;
    int maxStations;
    int clientsCount;
    int drones;
    int robots;
    int scooters;
    int totalPackages;
    int spawnFrequency;

    std::vector<std::vector<CellType>> map; // harta
    std::vector<Agent *> fleet;             // curierii

    std::vector<Package> activePackages; // pachetele active din sistem la fiecare tick

    std::vector<Position> clientPositions; // vector cu coordonatele tuturor destinatarilor
    int packagesGeneratedCount = 0;
    void spawnPackages(int currentTick); // cream pachete

    std::vector<Position> findPath(Position start, Position end, bool ignoreWalls);

    long long currentProfit = 0; // profitul total, chiar daca este negativ din pacate, am tot incercat sa indrept asta 
    int deliveredCount = 0;
    int missedCount = 0;
    Position hubPosition{0, 0};

    std::vector<Package> packageHistory; // istoric pachete
    std::vector<int> agentDeliveries;    // nr pachete livrate per agent

    std::vector<std::string> snapshotStart;
    std::vector<std::string> snapshotEnd;

    std::vector<std::string> renderMapState() const;
    void writeMapSnapshot(std::ofstream &out, const std::string &title, const std::vector<std::string> &snapshot);
    void printMapToConsole();

    void updateAgents(int currentTick);
    void assignTasks();

public:
    SimulationManager(const SimulationManager &) = delete;
    SimulationManager &operator=(const SimulationManager &) = delete;

    static SimulationManager &getInstance();

    void initialize();
    void run();

    ~SimulationManager()
    {
        for (auto agent : fleet){
            delete agent;
        }
        
        fleet.clear();
    }

    void generateReport();
};