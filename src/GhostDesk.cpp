#include "../include/ghostdesk_api.h"
#include <shellapi.h>

#define ID_TOGGLE 1001
#define ID_EXIT 1002
#define ID_AUTOSTART 1003
#define ID_SETTINGS 1004
#define WM_TRAYICON (WM_USER + 1)
#define TIMER_ID 1

static HWND mainWindow;
static bool mouseAtBottom = false;

bool IsMouseAtAnyScreenBottom() {
    POINT cursor;
    GetCursorPos(&cursor);
    
    HMONITOR hMonitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    
    if (GetMonitorInfo(hMonitor, &mi)) {
        return (cursor.y >= mi.rcMonitor.bottom - 10);
    }
    
    return (cursor.y >= GetSystemMetrics(SM_CYSCREEN) - 10);
}

void CheckMousePosition() {
    if (!IsDesktopHidden()) {
        mouseAtBottom = false;
        return;
    }
    
    bool currentlyAtBottom = IsMouseAtAnyScreenBottom();
    
    if (currentlyAtBottom && !mouseAtBottom) {
        ShowTaskbarAnimated();
        mouseAtBottom = true;
    } else if (!currentlyAtBottom && mouseAtBottom) {
        HideTaskbarAnimated();
        mouseAtBottom = false;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateWindowA("BUTTON", "Toggle Desktop", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        10, 10, 120, 30, hwnd, (HMENU)ID_TOGGLE, NULL, NULL);
            CreateWindowA("BUTTON", "Auto Start", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                        10, 50, 120, 20, hwnd, (HMENU)ID_AUTOSTART, NULL, NULL);
            CreateWindowA("BUTTON", "Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        10, 80, 60, 25, hwnd, (HMENU)ID_EXIT, NULL, NULL);
            
            CheckDlgButton(hwnd, ID_AUTOSTART, IsAutoStartEnabled() ? BST_CHECKED : BST_UNCHECKED);
            break;
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_TOGGLE:
                    ToggleDesktop();
                    break;
                case ID_AUTOSTART:
                    SetAutoStart(IsDlgButtonChecked(hwnd, ID_AUTOSTART) == BST_CHECKED);
                    break;
                case ID_EXIT:
                    RestoreDesktop();
                    PostQuitMessage(0);
                    break;
            }
            break;
            
        case WM_HOTKEY:
            if (wParam == 1) ToggleDesktop();
            else if (wParam == 2) {
                RestoreDesktop();
                RemoveSystemTray();
                UnregisterGlobalHotkeys(hwnd);
                PostQuitMessage(0);
            }
            break;
            
        case WM_TIMER:
            if (wParam == TIMER_ID) CheckMousePosition();
            break;
            
        case WM_TRAYICON:
            if (lParam == WM_LBUTTONDBLCLK) {
                ShowWindow(hwnd, IsWindowVisible(hwnd) ? SW_HIDE : SW_SHOW);
            } else if (lParam == WM_RBUTTONUP) {
                HMENU hMenu = CreatePopupMenu();
                AppendMenuA(hMenu, MF_STRING, ID_TOGGLE, IsDesktopHidden() ? "Show" : "Hide");
                AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuA(hMenu, MF_STRING, ID_EXIT, "Exit");
                
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
            }
            break;
            
        case WM_SYSCOMMAND:
            if (wParam == SC_MINIMIZE) {
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            break;
            
        case WM_DESTROY:
            RestoreDesktop();
            RemoveSystemTray();
            UnregisterGlobalHotkeys(hwnd);
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    if (!InitDesktopControl()) {
        MessageBoxA(NULL, "Init failed", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GhostDeskApp";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);
    
    mainWindow = CreateWindowA("GhostDeskApp", "GhostDesk Control Panel", 
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                            CW_USEDEFAULT, CW_USEDEFAULT, 160, 140,
                            NULL, NULL, hInstance, NULL);
    
    // Multi-monitor support ready
    
    RegisterGlobalHotkeys(mainWindow);
    CreateSystemTray(mainWindow);
    SetTimer(mainWindow, TIMER_ID, 250, NULL); // Reduced frequency
    
    // Auto-hide desktop on startup
    ToggleDesktop();
    
    ShowWindow(mainWindow, SW_HIDE); // Start minimized to tray
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}