#pragma once

#include <iostream>
#include <string>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "MainWindow.h"
#include "RobotoMediumFont.h"


static const char* dropped_file_path = nullptr;
static size_t dropped_file_path_length = 0;
#define EXTERNAL_PAYLOAD_TYPE "FILES"

static void DropCallback(GLFWwindow*, int count, const char** paths)
{
    if (count > 0) {
        dropped_file_path = strdup(paths[0]);
        dropped_file_path_length = strlen(dropped_file_path) + 1;
    }
}

class GuiManager
{
public:
    GuiManager(int32_t width, int32_t height, const std::string& title);
    virtual ~GuiManager();

    void update();
    void render();

    void draw();

    bool is_close();

private:
    ImVec4 clear_color;
    GLFWwindow* window;

    MainWindow main_window;
};

GuiManager::GuiManager(int32_t width, int32_t height, const std::string& title)
{
    if (!glfwInit()) throw std::runtime_error("Error Initialize glfwInit");
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Error Initialize glfwCreateWindow");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetDropCallback(window, DropCallback);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
//    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = nullptr;
//    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 20.0f);
//    io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 18.0f);
    ImFontConfig font_cfg = ImFontConfig();
    font_cfg.FontDataOwnedByAtlas = false;
    io.FontDefault = io.Fonts->AddFontFromMemoryTTF(FONT_DATA, FONT_SIZE, 12.0f, &font_cfg);
    io.Fonts->AddFontFromMemoryTTF(FONT_DATA, FONT_SIZE, 15.0f, &font_cfg);

    ImGui::StyleColorsLight();

    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

GuiManager::~GuiManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void GuiManager::update()
{

}

void GuiManager::render()
{
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        const ImGuiPayload* payload = ImGui::GetDragDropPayload();
        bool userIsDraggingExternalData = payload != nullptr && payload->IsDataType(EXTERNAL_PAYLOAD_TYPE);
        if ((userIsDraggingExternalData || dropped_file_path != nullptr) && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern)) {
            if (dropped_file_path) {
                ImGui::SetDragDropPayload(EXTERNAL_PAYLOAD_TYPE, dropped_file_path, dropped_file_path_length, ImGuiCond_Once);
                free((void*)dropped_file_path);
                dropped_file_path = nullptr;
                dropped_file_path_length = 0;
            }
            ImGui::TextUnformatted("   ");
            ImGui::EndDragDropSource();
        }
    }

    this->draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool GuiManager::is_close()
{
    return !glfwWindowShouldClose(window);
}

void GuiManager::draw()
{
    main_window.Draw();
}
