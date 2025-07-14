#include "../include/ghostdesk_api.h"
#include <shellapi.h>

#define ID_TOGGLE 1001
#define ID_EXIT 1002
#define ID_AUTOSTART 1003
#define ID_SETTINGS 1004
#define WM_TRAYICON (WM_USER + 1)
#define TIMER_ID 1

HWND mainWindow;
bool autoHideEnabled = true;

void CheckMousePosition() {
    if (!IsDesktopHidden()) return;
    
    POINT cursor;
    GetCursorPos(&cursor);
    
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(monitor, &mi);
    
    static bool taskbarShown = false;
    if (cursor.y >= mi.rcMonitor.bottom - 5 && !taskbarShown) {
        // Show taskbar temporarily
        taskbarShown = true;
    } else if (cursor.y < mi.rcMonitor.bottom - 50 && taskbarShown) {
        taskbarShown = false;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateWindowA("BUTTON", "隱藏/顯示桌面", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        10, 10, 120, 30, hwnd, (HMENU)ID_TOGGLE, NULL, NULL);
            CreateWindowA("BUTTON", "開機自動啟動", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                        10, 50, 120, 20, hwnd, (HMENU)ID_AUTOSTART, NULL, NULL);
            CreateWindowA("BUTTON", "退出", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
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
                PostQuitMessage(0);
            }
            break;
            
        case WM_TIMER:
            if (wParam == TIMER_ID && autoHideEnabled) CheckMousePosition();
            break;
            
        case WM_TRAYICON:
            if (lParam == WM_LBUTTONDBLCLK) {
                ShowWindow(hwnd, IsWindowVisible(hwnd) ? SW_HIDE : SW_SHOW);
            } else if (lParam == WM_RBUTTONUP) {
                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING, ID_TOGGLE, IsDesktopHidden() ? "顯示桌面" : "隱藏桌面");
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_STRING, ID_EXIT, "退出");
                
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
            RemoveSystemTray();
            UnregisterGlobalHotkeys(hwnd);
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    if (!InitDesktopControl()) {
        MessageBoxA(NULL, "初始化失敗", "錯誤", MB_OK | MB_ICONERROR);
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
    
    RegisterGlobalHotkeys(mainWindow);
    CreateSystemTray(mainWindow);
    SetTimer(mainWindow, TIMER_ID, 100, NULL);
    
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