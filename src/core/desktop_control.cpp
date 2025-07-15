#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"

static bool isHidden = false;
static bool taskbarTempVisible = false;
static HWND desktopIcons = NULL;
static HWND taskbar = NULL;
HWND taskbars[10] = {0};
int taskbarCount = 0;

BOOL InitDesktopControl() {
    taskbar = FindWindowA("Shell_TrayWnd", NULL);
    taskbars[0] = taskbar;
    taskbarCount = 1;
    
    HWND secondary = NULL;
    while ((secondary = FindWindowExA(NULL, secondary, "Shell_SecondaryTrayWnd", NULL)) != NULL && taskbarCount < 10) {
        taskbars[taskbarCount++] = secondary;
    }
    
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
    for (int i = 0; i < taskbarCount; i++) {
        ShowWindow(taskbars[i], cmd);
    }
    ShowWindow(desktopIcons, cmd);
    isHidden = !isHidden;
    taskbarTempVisible = false;
}

BOOL IsDesktopHidden() {
    return isHidden;
}

void RestoreDesktop() {
    if (isHidden) {
        for (int i = 0; i < taskbarCount; i++) {
            ShowWindow(taskbars[i], SW_SHOW);
        }
        ShowWindow(desktopIcons, SW_SHOW);
        isHidden = false;
        taskbarTempVisible = false;
    }
}

void ShowTaskbarAnimated() {
    if (isHidden && !taskbarTempVisible) {
        for (int i = 0; i < taskbarCount; i++) {
            ShowWindow(taskbars[i], SW_SHOW);
            SetLayeredWindowAttributes(taskbars[i], 0, 0, LWA_ALPHA);
            SetWindowLongA(taskbars[i], GWL_EXSTYLE, GetWindowLongA(taskbars[i], GWL_EXSTYLE) | WS_EX_LAYERED);
            
            for (int alpha = 0; alpha <= 255; alpha += 15) {
                SetLayeredWindowAttributes(taskbars[i], 0, alpha, LWA_ALPHA);
                Sleep(10);
            }
        }
        taskbarTempVisible = true;
    }
}

void HideTaskbarAnimated() {
    if (isHidden && taskbarTempVisible) {
        for (int i = 0; i < taskbarCount; i++) {
            for (int alpha = 255; alpha >= 0; alpha -= 15) {
                SetLayeredWindowAttributes(taskbars[i], 0, alpha, LWA_ALPHA);
                Sleep(10);
            }
            ShowWindow(taskbars[i], SW_HIDE);
            SetWindowLongA(taskbars[i], GWL_EXSTYLE, GetWindowLongA(taskbars[i], GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
        taskbarTempVisible = false;
    }
}