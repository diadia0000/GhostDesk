#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"
#include <vector>

static bool isHidden = false;
static HWND desktopIcons = NULL;
static std::vector<HWND> taskbars;

BOOL InitDesktopControl() {
    taskbars.clear();
    taskbars.push_back(FindWindowA("Shell_TrayWnd", NULL));
    
    HWND secondary = NULL;
    while ((secondary = FindWindowExA(NULL, secondary, "Shell_SecondaryTrayWnd", NULL)) != NULL) {
        taskbars.push_back(secondary);
    }
    
    HWND progman = FindWindowA("Progman", NULL);
    desktopIcons = FindWindowExA(progman, NULL, "SHELLDLL_DefView", NULL);
    desktopIcons = FindWindowExA(desktopIcons, NULL, "SysListView32", NULL);
    
    return (desktopIcons != NULL && !taskbars.empty());
}

void ToggleDesktop() {
    if (!isHidden) {
        for (HWND tb : taskbars) ShowWindow(tb, SW_HIDE);
        ShowWindow(desktopIcons, SW_HIDE);
        isHidden = true;
    } else {
        for (HWND tb : taskbars) ShowWindow(tb, SW_SHOW);
        ShowWindow(desktopIcons, SW_SHOW);
        isHidden = false;
    }
}

BOOL IsDesktopHidden() {
    return isHidden;
}

void RestoreDesktop() {
    if (isHidden) {
        for (HWND tb : taskbars) ShowWindow(tb, SW_SHOW);
        ShowWindow(desktopIcons, SW_SHOW);
        isHidden = false;
    }
}