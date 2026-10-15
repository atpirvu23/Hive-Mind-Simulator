#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "SimulationManager.h"

using namespace std;

int main()
{
    // apelam singleton-ul SimulationManager
    SimulationManager &sim = SimulationManager::getInstance();

    // initializam "aplicatia"
    sim.initialize();

    // pornim simularea
    sim.run();

    cout << endl << "Apasa Enter pentru a iesi...";

    cin.get();

    return 0;
}