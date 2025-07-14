#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"

static bool isHidden = false;
static HWND desktopIcons = NULL;
static HWND taskbar = NULL;
static HWND secondaryTaskbar = NULL;

BOOL InitDesktopControl() {
    taskbar = FindWindowA("Shell_TrayWnd", NULL);
    secondaryTaskbar = FindWindowExA(NULL, NULL, "Shell_SecondaryTrayWnd", NULL);
    
    HWND progman = FindWindowA("Progman", NULL);
    if (progman) {
        desktopIcons = FindWindowExA(progman, NULL, "SHELLDLL_DefView", NULL);
        if (desktopIcons) {
            desktopIcons = FindWindowExA(desktopIcons, NULL, "SysListView32", NULL);
        }
    }
    
    return (desktopIcons && taskbar);
}

void ToggleDesktop() {
    int cmd = isHidden ? SW_SHOW : SW_HIDE;
    ShowWindow(taskbar, cmd);
    if (secondaryTaskbar) ShowWindow(secondaryTaskbar, cmd);
    ShowWindow(desktopIcons, cmd);
    isHidden = !isHidden;
}

BOOL IsDesktopHidden() {
    return isHidden;
}

void RestoreDesktop() {
    if (isHidden) {
        ShowWindow(taskbar, SW_SHOW);
        if (secondaryTaskbar) ShowWindow(secondaryTaskbar, SW_SHOW);
        ShowWindow(desktopIcons, SW_SHOW);
        isHidden = false;
    }
}