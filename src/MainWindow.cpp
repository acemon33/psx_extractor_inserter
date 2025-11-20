#include "MainWindow.h"

#include <iostream>
#include <fstream>
#include <utility>
#include <cmath>
#include <filesystem>
#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "AboutImage.h"
#include "CoreOperation.h"
#include "Utilities.h"


uint64_t get_size(uint64_t bytes, int size_type)
{
    uint64_t result;
    if (size_type == 0)
        result = std::ceil((double)bytes / DATA_SECTOR);
    else
        result = bytes * DATA_SECTOR;
    return result;
}

uint64_t calculate_position(uint64_t address, int address_type)
{
    return (address_type == 0) ? address * SECTOR : (address / SECTOR) * SECTOR;
}

uint64_t calculate_size(uint64_t size, int address_type)
{
    return (address_type == 1) ? std::ceil((double) size / DATA_SECTOR) : size;
}

void open_external_link(const char* link)
{
#ifdef WIN32
    ShellExecuteA(nullptr, "open", link, nullptr, nullptr, SW_SHOWNORMAL);
#else
    system(("xdg-open " + std::string(link)).c_str());
#endif
}

void MainWindow::Draw()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    /// @begin TopWindow
    bool tmpOpen = true;
    ImGui::Begin("PSX Game File Extractor/Inserter", &tmpOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    {
        this->draw_main_widgets();
        this->draw_table_files();
        this->draw_popup();

        /// @begin MenuBar
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("About", nullptr, &about_me);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        /// @end MenuBar

        this->draw_about_me();

//        bool show_demo_window = true;
//        ImGui::ShowDemoWindow(&show_demo_window);

        /// @separator
        ImGui::End();
    }
    /// @end TopWindow
}

void MainWindow::extract_game_file()
{
    std::fstream game_file(this->game_filename, std::ios::in | std::ios::binary);
    if(!game_file.is_open())
        Utilities::handle_error("Cannot open the Game filename: " + this->game_filename);
    this->status_message = "These Files has been Extracted Successfully\n\n";

    for (auto & e : this->file_list) {
        if (e->filename.empty() || !e->is_checked)
            continue;

        uint64_t position = calculate_position(e->address, e->address_format);
        uint64_t n = calculate_size(e->size, e->size_format);
        uint64_t max_size = n * SECTOR;

        CdSector* data = CoreOperation::readSectors(game_file, position, n, max_size);
        CoreOperation::writeFile(e->filename, data, n);

        delete[] data;
        this->status_message += " - " + e->filename;
    }
    this->status_message += "\n\n";
    game_file.close();
}

void MainWindow::insert_game_file()
{
    std::fstream game_file(this->game_filename, std::ios::in | std::ios::out | std::ios::binary);
    if(!game_file.is_open())
        Utilities::handle_error("Cannot open the game_filename: " + this->game_filename);

    this->status_message = "The Game file: " + this->game_filename + " updated Successfully.\nThese Files has been Inserted Successfully.\n\n";
    for (auto & e : this->file_list) {
        if (e->filename.empty() || !std::filesystem::exists(e->filename) || !e->is_checked)
            continue;

        uint64_t position = calculate_position(e->address, e->address_format);
        uint64_t n = calculate_size(e->size, e->size_format);
        uint64_t max_size = n * SECTOR;

        char* data = CoreOperation::readFile(e->filename, max_size);
        CoreOperation::writeSectors(game_file, position, data, n, max_size);

        delete[] data;
        this->status_message += " - " + e->filename;
    }
    this->status_message += "\n\n";
    game_file.close();
}

void MainWindow::draw_about_me()
{
    if (about_me) {
        ImGui::OpenPopup("About Me");
        about_me = false;
    }
    if (ImGui::BeginPopupModal("About Me", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::TextUnformatted("PSX Game File Extractor / Inserter");
        ImGui::TextUnformatted("v2.0");

        ImGui::BeginGroup();
        unsigned char* image_data = nullptr;
        int w, h;
        image_data = stbi_load_from_memory(ABOUT_ME_IMAGE, ABOUT_ME_IMAGE_SIZE, &w, &h, nullptr, 4);
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        auto id = (ImTextureID)(intptr_t)image_texture;
        glBindTexture(GL_TEXTURE_2D, image_texture);
        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);
        ImGui::Image(id, {ABOUT_ME_IMAGE_WIDTH - 10, ABOUT_ME_IMAGE_HEIGHT - 10 });

        ImGui::SameLine();
        ImGui::BeginGroup();

        ImGui::TextUnformatted("Developed by");
        ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, 0xffff0000);
        ImGui::TextUnformatted("acemon33\n"); ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::TextUnformatted("Email: ");
        ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, 0xffff0000);
        ImGui::TextUnformatted("zaxmon33@gmail.com"); ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked()) open_external_link("mailto:zaxmon33@gmail.com");

        ImGui::Spacing();
        ImGui::TextUnformatted("Github: ");
        ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, 0xffff0000);
        ImGui::TextUnformatted("https://github.com/acemon33"); ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked()) open_external_link("https://github.com/acemon33");

        ImGui::Spacing();
        ImGui::TextUnformatted("Discord: ");
        ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, 0xffff0000);
        ImGui::TextUnformatted("acemon33#3218"); ImGui::PopStyleColor();

        ImGui::EndGroup();
        ImGui::EndGroup();

        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        ImGui::TextUnformatted("Created by using:");
        ImGui::TextUnformatted(" - ImGui v1.90.1"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, 0xffff0000);
        ImGui::TextUnformatted("https://github.com/ocornut/imgui"); ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked()) open_external_link("https://github.com/ocornut/imgui");

        ImGui::Spacing();
        ImGui::TextUnformatted(" - ImRAD v0.7"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, 0xffff0000);
        ImGui::TextUnformatted("https://github.com/tpecholt/imrad"); ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked()) open_external_link("https://github.com/tpecholt/imrad");

        ImGui::Spacing();
        ImGui::TextUnformatted(" - TINY FILE DIALOGS"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, 0xffff0000);
        ImGui::TextUnformatted("https://sourceforge.net/projects/tinyfiledialogs"); ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked()) open_external_link("https://sourceforge.net/projects/tinyfiledialogs");

        ImGui::Spacing();
        ImGui::TextUnformatted(" - GLFW v3.21.2"); ImGui::SameLine();

        ImGui::SameLine(); ImGui::Indent(350.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
        if (ImGui::Button("Close", { 100, 0 }))
            ImGui::CloseCurrentPopup();
        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
}

void MainWindow::draw_popup()
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Success", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::Text(this->status_message.c_str());
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::Text("%s\n\n\n", status_message.c_str());
        if (ImGui::Button("Close", ImVec2(100, 0))) { ImGui::CloseCurrentPopup(); status_message = ""; }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}

void MainWindow::draw_table_files()
{
    const ImU32 u32_one = 1;
    /// @begin Table
    ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();
    ImGui::Indent(-ImGui::GetStyle().IndentSpacing * 3);
    if (ImGui::BeginTable("table1", 3, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV, { -1, 0 })) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_WidthStretch, 0);
        ImGui::TableSetupColumn("                                                                    ", ImGuiTableColumnFlags_WidthFixed, 0);
        ImGui::TableHeadersRow();

        for (int i = 0; i < this->file_list.size(); i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::PushID(i);
            ImGui::Text("%d", i + 1);

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(295);
            ImGui::InputText("##filename", &this->file_list[i]->filename);
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILES")) {
                    this->file_list[i]->filename = (const char*)payload->Data;
                    if (std::filesystem::exists(this->file_list[i]->filename))
                        this->file_list[i]->size = get_size(std::filesystem::file_size(this->file_list[i]->filename), this->file_list[i]->size_format);
                }
            }
            ImGui::Checkbox("##active", &this->file_list[i]->is_checked);
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
            ImGui::PushStyleColor(ImGuiCol_Button, 0xffb8a218);
            ImGui::Button("Browse###filename_browse_button", { 50, 0 });
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            if (ImGui::IsItemClicked()) {
                char* temp = tinyfd_saveFileDialog("Open / Save a File", this->file_list[i]->filename.c_str(), 1, nullptr, nullptr);
                if (temp) {
                    this->file_list[i]->filename = temp;
                    if (std::filesystem::exists(this->file_list[i]->filename))
                        this->file_list[i]->size = get_size(std::filesystem::file_size(this->file_list[i]->filename), this->file_list[i]->size_format);
                }
            }
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
            ImGui::PushStyleColor(ImGuiCol_Button, 0xff6b79cc);
            ImGui::Button("Remove###filename_browse_button2", { 50, 0 });
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            if (ImGui::IsItemClicked())
                this->file_list.erase(this->file_list.begin() + i);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Address:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            ImGui::InputScalar("##address", ImGuiDataType_U32, &this->file_list[i]->address, &u32_one, nullptr, (this->file_list[i]->address_format == 0) ? "%u" : "%x");

            ImGui::SameLine();
            if (ImGui::RadioButton("LBA", &this->file_list[i]->address_format, 0)) {
                if (this->file_list[i]->address_format == 0)
                    this->file_list[i]->address = this->file_list[i]->address / SECTOR;
                else
                    this->file_list[i]->address = this->file_list[i]->address * SECTOR;
            }
            ImGui::SetItemTooltip("LBA value in DECIMAL");
            ImGui::SameLine();
            if (ImGui::RadioButton("Offset", &this->file_list[i]->address_format, 1)) {
                if (this->file_list[i]->address_format == 0)
                    this->file_list[i]->address = this->file_list[i]->address / SECTOR;
                else
                    this->file_list[i]->address = this->file_list[i]->address * SECTOR;
            }
            ImGui::SetItemTooltip("Offset value in HEX");

            ImGui::TextUnformatted("Size: ");
            if (this->file_list[i]->size < 1) this->file_list[i]->size = 1;
            ImGui::SameLine(0, 3 * ImGui::GetStyle().ItemSpacing.x );
            ImGui::SetNextItemWidth(120);
            ImGui::InputScalar("##size", ImGuiDataType_U32, &this->file_list[i]->size, &u32_one, nullptr, (this->file_list[i]->size_format == 0) ? "%u" : "%x");
            ImGui::SameLine();
            if (ImGui::RadioButton("Sector", &this->file_list[i]->size_format, 0))
                this->file_list[i]->size = get_size(this->file_list[i]->size, this->file_list[i]->size_format);
            ImGui::SetItemTooltip("Sector value in DECIMAL");
            ImGui::SameLine();
            if (ImGui::RadioButton("Byte", &this->file_list[i]->size_format, 1))
                this->file_list[i]->size = get_size(this->file_list[i]->size, this->file_list[i]->size_format);
            ImGui::SetItemTooltip("Byte value in HEX");
            ImGui::PopID();
        }

        /// @separator
        ImGui::EndTable();
    }
    /// @end Table
}

void MainWindow::draw_main_widgets()
{
    /// @separator
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

    /// @begin Text
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Indent();
    ImGui::TextUnformatted("Image:");
    /// @end Text

    /// @begin Input
    ImGui::SameLine();
    ImGui::SetNextItemWidth(450);
    ImGui::InputText("##game_filename", &game_filename);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILES"))
            this->game_filename = (const char*)payload->Data;
    }
    /// @end Input

    /// @begin Button
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
    ImGui::PushStyleColor(ImGuiCol_Button, 0xffb8a218);
    ImGui::Button("Browse###game_file_browse_button", { 100, 20 });
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    if (ImGui::IsItemClicked()) {
        static char const* fileFilterPattern[3] = {"*.bin", "*.img", "*.iso" };
        char* temp = tinyfd_openFileDialog("Select Game File", nullptr, 3, fileFilterPattern, "Image files", 1);
        if (temp)
            this->game_filename = temp;
    }
    /// @end Button

    /// @begin Button
    ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();
    ImGui::Indent();ImGui::Indent();
    int selected_files = 0;
    for (auto & e : this->file_list)
        if (e->is_checked)
            selected_files++;
    ImGui::Text("Selected Files: %d  ", selected_files);
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
    ImGui::PushStyleColor(ImGuiCol_Button, 0xff00ffff);
    ImGui::Button("Add File", { 115, 20 });
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::SetItemTooltip("Click to add a file entry or Drag and Drop a file here");
    if (ImGui::IsItemClicked())
        file_list.push_back(std::make_shared<FileInfo>("", 0, 0, 1, 0));
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILES")) {
            auto filename = (const char*)payload->Data;
            if (std::filesystem::exists(filename))
                file_list.push_back(std::make_shared<FileInfo>(filename, 0, 0, get_size(std::filesystem::file_size(filename), 0), 0));
        }
    }
    /// @begin Button

    /// @begin Button
    ImGui::SameLine();
    ImGui::BeginDisabled(selected_files < 1);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
    ImGui::PushStyleColor(ImGuiCol_Button, 0xffdb79cc);
    ImGui::Button("Extract", { 115, 20 });
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    if (ImGui::IsItemClicked()) {
        try {
            this->extract_game_file();
            ImGui::OpenPopup("Success");
        } catch(std::runtime_error &e) {
            ImGui::OpenPopup("Error");
            status_message = e.what();
        }
    }
    /// @end Button

    /// @begin Button
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
    ImGui::PushStyleColor(ImGuiCol_Button, 0xff45a728);
    ImGui::Button("Insert", { 115, 20 });
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    if (ImGui::IsItemClicked()) {
        try {
            this->insert_game_file();
            ImGui::OpenPopup("Success");
        } catch(std::runtime_error &e) {
            ImGui::OpenPopup("Error");
            status_message = e.what();
        }
    }
    /// @end Button
    ImGui::PopFont();
    ImGui::EndDisabled();
}
