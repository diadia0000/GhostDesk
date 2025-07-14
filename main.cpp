#include <windows.h>
#include <vector>

#define HOTKEY_TOGGLE 1
#define HOTKEY_EXIT 2
#define TIMER_ID 1

bool isHidden = false;
HWND desktopIcons;
std::vector<HWND> taskbars;

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

void CheckMousePosition() {
    if (!isHidden) return;
    
    POINT cursor;
    GetCursorPos(&cursor);
    
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(monitor, &mi);
    
    if (cursor.y >= mi.rcMonitor.bottom - 5) {
        for (HWND tb : taskbars) ShowWindow(tb, SW_SHOW);
    } else if (cursor.y < mi.rcMonitor.bottom - 50) {
        for (HWND tb : taskbars) ShowWindow(tb, SW_HIDE);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_HOTKEY:
            if (wParam == HOTKEY_TOGGLE) {
                ToggleDesktop();
            } else if (wParam == HOTKEY_EXIT) {
                if (isHidden) {
                    for (HWND tb : taskbars) ShowWindow(tb, SW_SHOW);
                    ShowWindow(desktopIcons, SW_SHOW);
                }
                PostQuitMessage(0);
            }
            break;
        case WM_TIMER:
            if (wParam == TIMER_ID) {
                CheckMousePosition();
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    taskbars.push_back(FindWindowA("Shell_TrayWnd", NULL));
    
    HWND secondary = NULL;
    while ((secondary = FindWindowExA(NULL, secondary, "Shell_SecondaryTrayWnd", NULL)) != NULL) {
        taskbars.push_back(secondary);
    }
    
    HWND progman = FindWindowA("Progman", NULL);
    desktopIcons = FindWindowExA(progman, NULL, "SHELLDLL_DefView", NULL);
    desktopIcons = FindWindowExA(desktopIcons, NULL, "SysListView32", NULL);
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GhostDesk";
    RegisterClassA(&wc);
    
    HWND hwnd = CreateWindowA("GhostDesk", "", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    
    RegisterHotKey(hwnd, HOTKEY_TOGGLE, MOD_CONTROL | MOD_SHIFT, 'D');
    RegisterHotKey(hwnd, HOTKEY_EXIT, MOD_CONTROL | MOD_SHIFT, 'Q');
    SetTimer(hwnd, TIMER_ID, 100, NULL);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnregisterHotKey(hwnd, HOTKEY_TOGGLE);
    UnregisterHotKey(hwnd, HOTKEY_EXIT);
    return 0;
}