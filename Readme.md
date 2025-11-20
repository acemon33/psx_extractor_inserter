# PSX Extractor-Inserter

A graphical tool to extract files from and insert files into PlayStation 1 (PSX) disc images (ISO Mode 2 / 2352 / Form 1).

This tool allows direct manipulation of files within a disc image by specifying the target location via LBA (Logical Block Addressing) or direct byte offset.

Unlike file-based tools, Its core function is to read and write full data sectors, making it suitable for modifying raw CD images.

![Screenshot of PSX Extractor-Inserter](screenshot.png) 

## Features

- **Extract & Insert:** Core functions to pull files from and push files into a PSX disc image.
- **Flexible Addressing:** Specify file locations by either LBA or an absolute byte offset.
- **Flexible Sizing:** Define file sizes by either sector count or byte count.
- **Drag & Drop:** Easily select your disc image and the file to insert by dragging them into the application window.
- **Auto Size Detection:** Automatically detects the size of the file you select to insert.

## Pre-Compiled Binaries

Can be found on [romhackplaza.org](https://romhackplaza.org/utilities/psx-extractor-inserter-utility) for Windows and Linux.


## Building from Source

This project uses CMake to generate build files.

### Prerequisites

- A C++17 compatible compiler (GCC/MinGW, Clang, or MSVC).
- CMake (version 3.21 or higher).

### Build Steps

```
git clone https://github.com/acemon33/psx_extractor_inserter.git
cd psx_extractor_inserter
mkdir build
cmake -S . -B build
cmake --build build --config Release

```

## Dependencies

This project relies on the following open-source libraries, which are included in the `third_party` directory:

-   [**Dear ImGui**](https://github.com/ocornut/imgui) v1.90.1
-   [**ImRAD**](https://github.com/tpecholt/imrad) v0.7
-   [**tinyfiledialogs**](https://sourceforge.net/projects/tinyfiledialogs) v3.21.2
-   [**stb_image**](https://github.com/nothings/stb) v2.30
-   [**GLFW**](https://github.com/glfw/glfw) v3.4

## License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](LICENSE) file for full details.

## Changelog

**v2.0**
- Support multiple files!
- Auto detect the size after selecting a file
- Fix Align wrong Address
- Fix Inserting / Extracting by Byte(s)

**v1.0**
- Operate mainly on complete data sectors of the disc image
- Support Drag and Drop files
- Address can be selected by LBA or Offset
- Size can be selected by Sector(s) or Byte(s)
- Extract a file from the Image
- Insert a file to the Image
