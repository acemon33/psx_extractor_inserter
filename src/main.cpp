#include "GuiManager.h"


int main()
{
    GuiManager win(640, 390, "PSX Game File Extractor/Inserter");

    while (win.is_close()) {
        win.update();
        win.render();
    }

    return 0;
}
