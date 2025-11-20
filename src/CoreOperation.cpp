#include "CoreOperation.h"

#include "Utilities.h"


char* CoreOperation::readFile(const std::string& file_path, uint64_t max_size)
{
    std::fstream file(file_path, std::ios::in | std::ios::binary);
    if(!file.is_open())
        Utilities::handle_error("Cannot open the game_filename: " + file_path);

    file.seekg(std::ios::beg, std::ios::end);
    int file_size = (int) file.tellg();
    char* data = new char[(max_size < file_size) ? file_size : max_size];
    memset(data, 0, (max_size < file_size) ? file_size : max_size);

    file.seekg(std::ios::beg);
    file.read(data, file_size);
    file.close();
    return data;
}

void CoreOperation::writeFile(const std::string& file_path, const CdSector* data, uint64_t n)
{
    std::fstream file(file_path, std::ios::out | std::ios::binary);

    if(!file.is_open())
        Utilities::handle_error("Cannot write the file: " + file_path);

    for (uint32_t i = 0; i < n; ++i)
        file.write(reinterpret_cast<const char *>(data[i].data), DATA_SECTOR);

    file.close();
}

CdSector* CoreOperation::readSectors(std::fstream& game_file, uint64_t position, uint64_t n, uint64_t max_size)
{
    CdSector* data = new CdSector[n];
    game_file.seekg(position, std::ios::beg);
    game_file.read(reinterpret_cast<char *>(data), max_size);
    return data;
}

void CoreOperation::writeSectors(std::fstream& game_file, uint64_t position, char* data, uint64_t n, uint64_t max_size)
{
    auto* result = new CdSector[n];
    game_file.seekg(position, std::ios::beg);
    game_file.read(reinterpret_cast<char *>(result), max_size);

    for (uint32_t i = 0; i < n; ++i)
        memcpy(result[i].data, data + (i * DATA_SECTOR), DATA_SECTOR);

    game_file.seekg(position, std::ios::beg);
    game_file.write(reinterpret_cast<char *>(result), max_size);

    delete[] result;
}
