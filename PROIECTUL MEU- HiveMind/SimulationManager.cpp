/*stiu ca apar multe erori in cod din pacate nu am reusit sa le rezolv pe toate din cauza timpului scurt
dar codul este functional iar executabilul merge 100%, sper cu toate cerintele complete*/

#include <fstream>
#include <iostream>
#include <sstream> //putem astfel trata un string ca pe un flux de intrare; ex: ss>>key>>value
#include <cstdlib> //pt random()
#include <queue>
#include <algorithm>
#include <chrono>

#include "AgentsDerived.h"
#include "MapGenerators.h"
#include "Package.h"
#include "SimulationManager.h"

using namespace std;

SimulationManager::SimulationManager()
    : mapWidth(0), mapHeight(0), maxTicks(0), maxStations(0),
      clientsCount(0), drones(0), robots(0), scooters(0),
      totalPackages(0), spawnFrequency(0)

{
}

SimulationManager &SimulationManager::getInstance()
{
    static SimulationManager instance;
    return instance;
}

void SimulationManager::initialize()
{
    ifstream file("simulation_setup.txt");
    if (!file.is_open())
    {
        cout << "Nu se poate deschide fisierul simulation_setup.txt" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string key;
        ss >> key;

        if (key == "MAP_SIZE:")
        {
            ss >> mapWidth >> mapHeight;
        }
        else if (key == "MAX_TICKS:")
        {
            ss >> maxTicks;
        }
        else if (key == "MAX_STATIONS:")
        {
            ss >> maxStations;
        }
        else if (key == "CLIENTS_COUNT:")
        {
            ss >> clientsCount;
        }
        else if (key == "DRONES:")
        {
            ss >> drones;
        }
        else if (key == "ROBOTS:")
        {
            ss >> robots;
        }
        else if (key == "SCOOTERS:")
        {
            ss >> scooters;
        }
        else if (key == "TOTAL_PACKAGES:")
        {
            ss >> totalPackages;
        }
        else if (key == "SPAWN_FREQUENCY:")
        {
            ss >> spawnFrequency;
        }
    }
    file.close();
    // generam harta
    ProceduralMapGenerator generator;
    map = generator.generate(mapWidth, mapHeight);

    {
        int hubs = 0, stations = 0, clients = 0;
        for (int i = 0; i < mapHeight; ++i)
        {
            for (int j = 0; j < mapWidth; ++j)
            {
                if (map[i][j] == HUB)
                    hubs++;
                else if (map[i][j] == STATION)
                    stations++;
                else if (map[i][j] == CLIENT)
                    clients++;
            }
        }
        using namespace std::chrono;
        long long ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    // facem flota goala si curatam memoria
    for (auto agent : fleet)
    {
        delete agent;
    }
    fleet.clear();

    // adaugam agentii in lista
    for (int i = 0; i < drones; ++i)
    {
        fleet.push_back(new Drone());
    }
    for (int i = 0; i < robots; ++i)
    {
        fleet.push_back(new Robot());
    }
    for (int i = 0; i < scooters; ++i)
    {
        fleet.push_back(new Scooter());
    }

    clientPositions.clear();
    hubPosition = {0, 0};

    for (int i = 0; i < mapHeight; ++i)
    {
        for (int j = 0; j < mapWidth; ++j)
        {
            if (map[i][j] == CLIENT)
            {
                clientPositions.push_back({i, j});
            }
            if (map[i][j] == HUB)
            {
                hubPosition = {i, j};
            }
        }
    }

    packagesGeneratedCount = 0;
    packageHistory.clear();

    // pozitionam agentii in hub
    for (auto *agent : fleet)
    {
        agent->setPosition(hubPosition.r, hubPosition.c);
    }
    agentDeliveries.assign(fleet.size(), 0);

    printMapToConsole();

    cout << "Initializare completa: Harta " << mapWidth << "x" << mapHeight << ", Agenti: " << fleet.size() << endl;
}
void SimulationManager::run()
{
    cout << "Simularea a inceput!" << endl
         << "Marimea hartii: " << mapWidth << " x " << mapHeight << endl
         << "Tick-uri maxime: " << maxTicks << endl
         << endl;

    snapshotStart = renderMapState();

    for (int currentTick = 1; currentTick <= maxTicks; ++currentTick)
    {
        spawnPackages(currentTick);
        assignTasks();             // le alocam
        updateAgents(currentTick); // actiunea curierilor

        if (currentTick % 100 == 0)
        {
            cout << "{Tick-ul" << currentTick << "} Simulare in curs..." << endl;
        }
    }

    cout << "Simulare terminata." << endl;

    snapshotEnd = renderMapState();
    generateReport();
}
void SimulationManager::spawnPackages(int currentTick)
{
    // verificam daca trebuie sa generam un pachet in acest tick, o data la 10 tick-uri
    if (currentTick % spawnFrequency == 0 && packagesGeneratedCount < totalPackages)
    {

        if (clientPositions.empty())
        {
            return;
        }

        // generam random informatiile pachetului
        int randIdx = rand() % clientPositions.size();
        Position dest = clientPositions[randIdx];

        int reward = 200 + (rand() % 601);
        int deadline = 10 + (rand() % 11);

        // constructor
        Package newPkg(packagesGeneratedCount, dest, reward, deadline, currentTick);

        activePackages.push_back(newPkg);
        packageHistory.push_back(newPkg);
        packagesGeneratedCount++;

        cout << "{Tick-ul " << currentTick << "} Pachet NOU generat! Destinatie: " << dest.r << "," << dest.c << " >> Reward: " << reward << endl;

        {
            using namespace std::chrono;
            static int logCount = 0;
            if (logCount < 6)
            {
                long long ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                logCount++;
            }
        }
    }
}

// algoritm BFS pentru a gasi cel mai scurt drum
vector<Position> SimulationManager::findPath(Position start, Position end, bool ignoreWalls)
{

    int dRow[] = {-1, 1, 0, 0};
    int dCol[] = {0, 0, -1, 1};

    // cream o matrice pentru a tine minte de unde am venit (pentru reconstructia drumului)
    vector<vector<Position>> parent(mapHeight, vector<Position>(mapWidth, {-1, -1}));

    // si inca una pt a stii unde am fost deja
    vector<vector<bool>> visited(mapHeight, vector<bool>(mapWidth, false));

    queue<Position> q;
    q.push(start);
    visited[start.r][start.c] = true;
    parent[start.r][start.c] = {-1, -1};
    bool found = false;

    while (!q.empty())
    {
        Position curr = q.front();
        q.pop();

        // test pt a vedea daca am ajuns la destinatie
        if (curr.r == end.r && curr.c == end.c)
        {
            found = true;
            break;
        }

        // exploram vecinii
        for (int i = 0; i < 4; i++)
        {
            int nr = curr.r + dRow[i];
            int nc = curr.c + dCol[i];

            if (nr >= 0 && nr < mapHeight && nc >= 0 && nc < mapWidth)
            {
                if (!visited[nr][nc])
                {
                    // verificam obstacolele
                    if (ignoreWalls || map[nr][nc] != WALL)
                    {
                        visited[nr][nc] = true;
                        q.push({nr, nc});
                        parent[nr][nc] = curr;
                    }
                }
            }
        }
    }

    // reconstruim drumul mergand inapoi
    vector<Position> path;
    if (found)
    {
        Position curr = end;
        while (curr.r != -1)
        {
            path.push_back(curr);
            if (curr.r == start.r && curr.c == start.c)
                break;
            curr = parent[curr.r][curr.c];
        }
        if (path.empty() || !(path.back() == start))
        {
            path.push_back(start);
        }
        reverse(path.begin(), path.end());
    }
    return path;
}

void SimulationManager::assignTasks()
{

    for (auto &pkg : activePackages)
    {
        if (pkg.assigned)
            continue; // verificam daca pachetul este deja luat

        // cautam curieri liberi
        for (auto *agent : fleet)
        {
            if (agent->getState() == IDLE && agent->isAlive())
            {

                bool ignoreWalls = (agent->getType() == "Drona");

                // calculam drumul
                vector<Position> path = findPath(agent->getPosition(), pkg.destination, ignoreWalls);

                if (!path.empty())
                {
                    agent->setPath(path);
                    agent->assignPackage(pkg.id);
                    agent->setState(MOVING); // schimbam starea manual

                    pkg.assigned = true;

                    cout << "HiveMind: Pachetul " << pkg.id << " alocat agentului " << agent->getType() << " (Drum: " << path.size() << " pasi)." << endl;

                    break; // dupa ce am gasit agent, iesim din bucla
                }
            }
        }
    }
}

void SimulationManager::updateAgents(int currentTick)
{
    // verificam pachetele expirate
    auto it = activePackages.begin();
    while (it != activePackages.end())
    {
        int age = currentTick - it->createdAt;
        if (age > it->deadline)
        {
            missedCount++;
            for (auto &pkgHist : packageHistory)
            {
                if (pkgHist.id == it->id)
                {
                    pkgHist.expired = true;
                    break;
                }
            }
            {
                using namespace std::chrono; // apelam chrono
                static int logCount = 0;
                if (logCount < 6)
                {
                    long long ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                    logCount++;
                }
            }
            it = activePackages.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (size_t idx = 0; idx < fleet.size(); ++idx)
    {
        auto *agent = fleet[idx];
        if (!agent->isAlive())
            continue;

        currentProfit -= agent->getCostPerTick();
        // incarcam bateria
        Position pos = agent->getPosition();
        if (map[pos.r][pos.c] == HUB || map[pos.r][pos.c] == STATION)
        {
            if (agent->getState() == IDLE || agent->getState() == CHARGING)
            {
                agent->charge();
                agent->setState(CHARGING);
                if (!agent->isLowBattery())
                {
                    agent->setState(IDLE);
                }
            }
        }

        // starea de miscare
        if (agent->hasPath())
        {
            Position nextPos = agent->moveNext();
            agent->setPosition(nextPos.r, nextPos.c);
            agent->consumeBattery();

            cout << "{Tick-ul " << currentTick << "} " << agent->getType() << " #" << idx << " s-a mutat la ("
                 << nextPos.r << "," << nextPos.c << ") Baterie: " << (agent->getBatteryLevel() * 100 / agent->getMaxBattery()) << "%";

            if (agent->getPackageId() != -1)
            {
                cout << " | Pachet: " << agent->getPackageId();
            }
            cout << endl;
            {
                using namespace std::chrono;
                static int logCount = 0;
                if (logCount < 8)
                {
                    long long ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                    logCount++;
                }
            }

            if (!agent->isAlive())
            {
                currentProfit = currentProfit-500;
                cout << "ALERTA: " << agent->getType() << " a murit (Baterie 0)!\n";
                continue;
            }
        }

        // livrarea pachetului
        if (agent->getPackageId() != -1 && !agent->hasPath())
        {
            int pkgId = agent->getPackageId();
            for (auto pkgIt = activePackages.begin(); pkgIt != activePackages.end(); ++pkgIt)
            {
                if (pkgIt->id == pkgId)
                {
                    currentProfit += pkgIt->reward;
                    deliveredCount++;
                    agentDeliveries[idx]++;
                    for (auto &pkgHist : packageHistory)
                    {
                        if (pkgHist.id == pkgIt->id)
                        {
                            pkgHist.delivered = true;
                            pkgHist.deliveredAt = currentTick;
                            break;
                        }
                    }
                    cout << "LIVRARE: Pachet-ul " << pkgId << " livrat! Profit curent: " << currentProfit << endl;
                    {
                        using namespace std::chrono;
                        static int logCount = 0;
                        if (logCount < 6)
                        {
                            long long ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                            logCount++;
                        }
                    }
                    activePackages.erase(pkgIt);
                    break;
                }
            }
            agent->clearPackage();
            agent->setState(IDLE);
        }
    }
}

void SimulationManager::generateReport()
{
    ofstream out("simulation.txt", std::ios::out | std::ios::trunc);
    if (!out.is_open())
    {
        cout << "Eroare la crearea raportului simulation.txt" << endl;
        return;
    }

    out << "RAPORT FINAL SIMULARE HIVEMIND:" << endl;
    out << "Dimensiune Harta: " << mapWidth << " x " << mapHeight << endl;
    out << "Durata Simulare: " << maxTicks << " tick-uri" << endl << endl;
    out << "Total Pachete Generate: " << packagesGeneratedCount << endl;
    out << "Pachete Livrate: " << deliveredCount << endl;
    out << "Pachete Ratate/In Curs: " << (packagesGeneratedCount - deliveredCount) << endl << endl;
    out << "PROFIT FINAL: " << currentProfit << " credite" << endl << endl << endl;
    out << "Statistici Agenti:" << endl;

    for (size_t idx = 0; idx < fleet.size(); ++idx)
    {
        out << "Agent #" << idx << " (" << fleet[idx]->getType() << "): Livrari="
            << agentDeliveries[idx] << ", Baterie finala="
            << fleet[idx]->getBatteryLevel() << "/" << fleet[idx]->getMaxBattery() << endl;
    }

    out << endl
        << "Istoric Pachete" << endl;
    for (const auto &pkg : packageHistory)
    {
        out << "Pachet " << pkg.id << " | Destinatie: (" << pkg.destination.r << "," << pkg.destination.c
            << ") >> Creat la tick " << pkg.createdAt;
        if (pkg.delivered)
        {
            out << " >>  Livrat la tick " << pkg.deliveredAt << endl;
        }
        else if (pkg.expired)
        {
            out << " >> Status: Expirat/Negasit" << endl;
        }
        else
        {
            out << " >> Status: Not Delivered" << endl;
        }
    }

    out << endl << "Snapshot Harta" << endl;
    writeMapSnapshot(out, snapshotStart);

    out.close();
    cout << "Raportul a fost salvat in 'simulation.txt'. Deschideti fisierul din folder!" << endl;
}

std::vector<std::string> SimulationManager::renderMapState() const
{
    std::vector<std::string> snapshot(mapHeight, std::string(mapWidth, '.'));
    for (int i = 0; i < mapHeight; ++i)
    {
        for (int j = 0; j < mapWidth; ++j)
        {
            char ch = '.';
            switch (map[i][j])
            {
            case EMPTY:
                ch = '.';
                break;
            case WALL:
                ch = '#';
                break;
            case HUB:
                ch = 'B';
                break;
            case STATION:
                ch = 'S';
                break;
            case CLIENT:
                ch = 'D';
                break;
            }
            snapshot[i][j] = ch;
        }
    }

    for (size_t idx = 0; idx < fleet.size(); ++idx)
    {
        Position p = fleet[idx]->getPosition();
        if (p.r >= 0 && p.r < mapHeight && p.c >= 0 && p.c < mapWidth)
        {
            char agentChar = 'A';
            std::string type = fleet[idx]->getType();
            if (type == "Drona")
                agentChar = 'd';
            else if (type == "Robot")
                agentChar = 'r';
            else if (type == "Scooter")
                agentChar = 's';
            if (snapshot[p.r][p.c] == '.')
                snapshot[p.r][p.c] = agentChar;
        }
    }

    return snapshot;
}

void SimulationManager::writeMapSnapshot(std::ofstream &out, const std::string &title, const std::vector<std::string> &snapshot)
{
    out << title << endl;
    for (const auto &row : snapshot)
    {
        out << row << endl;
    }
}

void SimulationManager::printMapToConsole()
{
    cout << "Harta generata procedural:" << endl;
    auto snapshot = renderMapState();
    for (const auto &row : snapshot)
    {
        cout << row << endl;
    }
    cout << endl;
}