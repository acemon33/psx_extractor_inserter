cmake_minimum_required(VERSION 3.21)
project(psx_extractor_inserter)

set(CMAKE_CXX_STANDARD 17)

include_directories(third_party)
include_directories(third_party/imgui)
include_directories(third_party/glfw/include)

if (WIN32)
  link_directories(third_party/glfw/lib-mingw)
else()
  link_directories(third_party/glfw/lib-nix)
  find_package(OpenGL REQUIRED)
endif()

file(GLOB_RECURSE project_src "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB imgui_src
        "third_party/imgui/*.cpp"
        "third_party/imgui/misc/cpp/imgui_stdlib.cpp"
        "third_party/imgui/backends/imgui_impl_glfw.cpp"
        "third_party/imgui/backends/imgui_impl_opengl3.cpp"
)
file(GLOB filedialog_src "third_party/tinyfiledialogs/tinyfiledialogs.c")

add_executable(${CMAKE_PROJECT_NAME} ${project_src} ${imgui_src} ${filedialog_src})

if (WIN32)
  target_link_libraries(${CMAKE_PROJECT_NAME} glfw3 mingw32 opengl32 user32 gdi32 kernel32 -mwindows -static)
else()
  target_link_libraries(${CMAKE_PROJECT_NAME} glfw3 OpenGL::GL)
endif()
