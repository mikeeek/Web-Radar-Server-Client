#define _WINSOCKAPI_  
#define WIN32_LEAN_AND_MEAN

#include "Include.hpp"
#include "radarclient.h"


INT WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, INT cmd_show)
{
 
    HWND consoleWind = GetConsoleWindow();
    if (consoleWind == NULL)
    {
        AllocConsole();
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);
        consoleWind = GetConsoleWindow();
        HMENU hmenu = GetSystemMenu(consoleWind, FALSE);
        EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
    }

    bool success = loadStuff();
    if (!success)
    {
        printf("Failed to init\n");
        Sleep(5000);
        return 1;
    }

 
	return 1;
}

