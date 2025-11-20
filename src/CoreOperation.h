#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "MainWindow.h"


namespace CoreOperation
{
    char* readFile(const std::string& file_path, uint64_t max_size);
    void writeFile(const std::string& file_path, const CdSector* data, uint64_t n);

    CdSector* readSectors(std::fstream& game_file, uint64_t position, uint64_t n, uint64_t max_size);
    void writeSectors(std::fstream& game_file, uint64_t position, char* data, uint64_t n, uint64_t max_size);
}
