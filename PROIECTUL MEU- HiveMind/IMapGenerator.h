#pragma once
#include <vector>
#include "Utils.h"

using namespace std;

class IMapGenerator
{
public:
    virtual ~IMapGenerator() {}

    virtual vector<vector<CellType>> generate(int width, int height) = 0;
    // se creeaza si returneaza harta sub forma de matrice de celule, respectand dimensiunile date la input (20x20)
};