#include "MapGenerators.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib> // pentru rand()
#include <ctime>

using namespace std;

vector<vector<CellType>> FileMapLoader::generate(int width, int height)
{
    vector<vector<CellType>> grid(height, vector<CellType>(width, EMPTY));

    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Eroare: Nu s-a putut deschide " << filename << ". Se genereaza o harta goala." << endl;
        return grid;
    }

    char ch;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (file >> ch)
            {
                switch (ch)
                {
                case '.':
                    grid[i][j] = EMPTY;
                    break;
                case '#':
                    grid[i][j] = WALL;
                    break;
                case 'B':
                    grid[i][j] = HUB;
                    break;
                case 'S':
                    grid[i][j] = STATION;
                    break;
                case 'D':
                    grid[i][j] = CLIENT;
                    break;
                default:
                    grid[i][j] = EMPTY;
                    break;
                }
            }
        }
    }

    file.close();
    return grid;
}

vector<vector<CellType>> ProceduralMapGenerator::generate(int width, int height)
{
    vector<vector<CellType>> grid(height, vector<CellType>(width, EMPTY));

    static bool seeded = false;
    if (!seeded)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }

    // alegem hub-ul in centru pt a fi mai usor
    int midRow = height / 2;
    int midCol = width / 2;
    grid[midRow][midCol] = HUB;

    // 3 statii de incarcare (S)
    int stationsPlaced = 0;
    while (stationsPlaced < 3)
    {
        int r = rand() % height, c = rand() % width;
        if (grid[r][c] == EMPTY)
        {
            grid[r][c] = STATION;
            stationsPlaced++;
        }
    }

    // 10 clienti (D)
    int clientsPlaced = 0;
    while (clientsPlaced < 10)
    {
        int r = rand() % height, c = rand() % width;
        if (grid[r][c] == EMPTY)
        {
            grid[r][c] = CLIENT;
            clientsPlaced++;
        }
    }

    // generam ziduri aleatoriu
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (grid[i][j] == EMPTY && rand() % 100 < 15)
            {
                grid[i][j] = WALL;
            }
        }
    }

    return grid;
}
