#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"

static bool isHidden = false;
static HWND desktopIcons = NULL;
static HWND taskbar = NULL;
HWND taskbars[10] = {0};
int taskbarCount = 0;
static bool taskbarVisible[10] = {false}; // Per-monitor visibility state
static bool animating[10] = {false}; // Per-monitor animation state

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
        taskbarVisible[i] = false;
        animating[i] = false;
    }
    ShowWindow(desktopIcons, cmd);
    isHidden = !isHidden;
}

BOOL IsDesktopHidden() {
    return isHidden;
}

void RestoreDesktop() {
    if (isHidden) {
        for (int i = 0; i < taskbarCount; i++) {
            ShowWindow(taskbars[i], SW_SHOW);
<<<<<<< HEAD
            taskbarVisible[i] = false;
            animating[i] = false;
=======
            SetWindowLongA(taskbars[i], GWL_EXSTYLE, GetWindowLongA(taskbars[i], GWL_EXSTYLE) & ~WS_EX_LAYERED);
>>>>>>> 8074f5b8583f56a69bf103e3e4fa208093f35da8
        }
        ShowWindow(desktopIcons, SW_SHOW);
        isHidden = false;
    }
}

void ShowTaskbarAnimated(int monitorIndex) {
    if (isHidden && monitorIndex >= 0 && monitorIndex < taskbarCount && 
        !taskbarVisible[monitorIndex] && !animating[monitorIndex]) {
        
        animating[monitorIndex] = true;
        HWND hwnd = taskbars[monitorIndex];
        
        ShowWindow(hwnd, SW_SHOW);
        SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLongA(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
        
        for (int alpha = 0; alpha <= 255; alpha += 25) {
            SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
            Sleep(8);
        }
        
        taskbarVisible[monitorIndex] = true;
        animating[monitorIndex] = false;
    }
}

void ShowAllTaskbarsAnimated() {
    if (!isHidden) return;
    bool anyVisible = false;
    for (int i = 0; i < taskbarCount; i++) {
        if (taskbarVisible[i]) { anyVisible = true; break; }
    }
    if (anyVisible) return;
    
    for (int i = 0; i < taskbarCount; i++) {
        ShowTaskbarAnimated(i);
    }
}

void HideTaskbarAnimated(int monitorIndex) {
    if (isHidden && monitorIndex >= 0 && monitorIndex < taskbarCount && 
        taskbarVisible[monitorIndex] && !animating[monitorIndex]) {
        
        animating[monitorIndex] = true;
        HWND hwnd = taskbars[monitorIndex];
        
        for (int alpha = 255; alpha >= 0; alpha -= 25) {
            SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
            Sleep(8);
        }
        
        ShowWindow(hwnd, SW_HIDE);
        SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLongA(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        
        taskbarVisible[monitorIndex] = false;
        animating[monitorIndex] = false;
    }
}

void HideAllTaskbarsAnimated() {
    if (!isHidden) return;
    bool anyVisible = false;
    for (int i = 0; i < taskbarCount; i++) {
        if (taskbarVisible[i]) { anyVisible = true; break; }
    }
    if (!anyVisible) return;
    
    for (int i = 0; i < taskbarCount; i++) {
        HideTaskbarAnimated(i);
    }
}