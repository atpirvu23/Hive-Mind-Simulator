#pragma once

#include "IMapGenerator.h"
#include <vector>
#include <string>

using namespace std;

// incarc harta dintr-un fisier text
class FileMapLoader : public IMapGenerator
{
private:
    string filename;

public:
    FileMapLoader(const std::string &file) : filename(file) {}

    std::vector<std::vector<CellType>> generate(int width, int height) override;
};

// genereaza harta procedural
class ProceduralMapGenerator : public IMapGenerator
{
public:
    std::vector<std::vector<CellType>> generate(int width, int height) override;
};
