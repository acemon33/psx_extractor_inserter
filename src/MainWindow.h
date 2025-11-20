#pragma once

#include <string>
#include <functional>

#include <imgui/imgui.h>
#include <memory>


static const uint16_t SECTOR = 2352;
static const uint16_t DATA_SECTOR = 2048;

struct CdSector
{
    char headers[24];
    char data[DATA_SECTOR];
    char edc_ecc[280];
};

struct FileInfo
{
    std::string filename;
    uint32_t address = 0;
    int address_format = 0;
    uint64_t size = 0;
    int size_format = 0;
    bool is_checked = true;

    FileInfo(std::string filename, int address, int address_format, int size, int size_format)
            : filename(std::move(filename)), address(address), address_format(address_format), size(size), size_format(size_format) {}
};

class MainWindow
{
public:
    void Draw();

private:
    void extract_game_file();
    void insert_game_file();

    void draw_about_me();
    void draw_main_widgets();
    void draw_popup();
    void draw_table_files();

    std::vector<std::shared_ptr<FileInfo>> file_list;
    std::string game_filename;
    std::string status_message;

    bool about_me = false;
};
