#include "../include/ghostdesk_api.h"
#include <shellapi.h>
#include <commdlg.h>

// 圖像加載器聲明
HBITMAP LoadJPEGFromFile(const char* filename);

#define IDB_BACKGROUND 102

#define ID_TOGGLE 1001
#define ID_EXIT 1002
#define ID_AUTOSTART 1003
#define ID_SETTINGS 1004
#define ID_SHOW_UI 1005
#define ID_BACKGROUND_BROWSE 1006
#define WM_TRAYICON (WM_USER + 1)
#define TIMER_ID 1

static HWND mainWindow;
static bool mouseAtBottom = false; // Global mouse state
// static HBITMAP hBackgroundBitmap = NULL; // 背景圖位圖 (未使用)
// static HDC hBackgroundDC = NULL; // 背景圖 DC (未使用)
static char customBackgroundPath[MAX_PATH] = ""; // 自訂背景路徑
static HWND hResourceLabel = NULL; // 系統資源顯示標籤

bool HasTaskbarPopup() {
    HWND notifyOverflow = FindWindowA("NotifyIconOverflowWindow", NULL);
    if (notifyOverflow && IsWindowVisible(notifyOverflow)) return true;
    
    return false;
}

int GetMouseMonitorIndex() {
    POINT cursor;
    GetCursorPos(&cursor);
    
    // 檢查滑鼠是否在任何工作列內部
    for (int i = 0; i < taskbarCount; i++) {
        RECT taskbarRect;
        if (GetWindowRect(taskbars[i], &taskbarRect)) {
            if (PtInRect(&taskbarRect, cursor)) {
                return i;
            }
        }
    }
    
    // 如果不在工作列內部，檢查是否在螢幕底部觸發區域
    HMONITOR hMonitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    
    for (int i = 0; i < taskbarCount; i++) {
        RECT taskbarRect;
        if (GetWindowRect(taskbars[i], &taskbarRect)) {
            HMONITOR taskbarMonitor = MonitorFromRect(&taskbarRect, MONITOR_DEFAULTTONEAREST);
            if (taskbarMonitor == hMonitor) {
                MONITORINFO mi = { sizeof(MONITORINFO) };
                if (GetMonitorInfo(hMonitor, &mi) && cursor.y >= mi.rcMonitor.bottom - 5) {
                    return i;
                }
            }
        }
    }
    return -1;
}

void CheckMousePosition() {
    if (!IsDesktopHidden()) {
        mouseAtBottom = false;
        return;
    }
    
    int currentMonitor = GetMouseMonitorIndex();
    bool hasPopup = HasTaskbarPopup();
    
    if (currentMonitor >= 0 && !mouseAtBottom) {
        ShowAllTaskbarsAnimated();
        mouseAtBottom = true;
    } else if (currentMonitor < 0 && mouseAtBottom && !hasPopup) {
        HideAllTaskbarsAnimated();
        mouseAtBottom = false;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            // 先填充背景色
            HBRUSH hBrush = CreateSolidBrush(RGB(45, 45, 48)); // 深灰色背景
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            
            // 加載背景圖 - 優先使用自訂背景
            HBITMAP hBitmap = NULL;
            if (strlen(customBackgroundPath) > 0) {
                hBitmap = (HBITMAP)LoadImageA(NULL, customBackgroundPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            }
            
            // 如果自訂背景載入失敗，使用預設背景
            if (!hBitmap) {
                hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND), 
                                           IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
            }
            
            if (hBitmap) {
                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
                
                BITMAP bm;
                GetObject(hBitmap, sizeof(bm), &bm);
                
                // 設定高品質縮放模式
                SetStretchBltMode(hdc, HALFTONE);
                SetBrushOrgEx(hdc, 0, 0, NULL);
                
                StretchBlt(hdc, 0, 0, rect.right, rect.bottom, 
                          memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
                
                SelectObject(memDC, oldBitmap);
                DeleteDC(memDC);
                DeleteObject(hBitmap);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_CREATE:
            CreateWindowA("BUTTON", "Toggle Desktop", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        10, 10, 120, 30, hwnd, (HMENU)ID_TOGGLE, NULL, NULL);
            CreateWindowA("BUTTON", "Auto Start", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                        10, 50, 120, 20, hwnd, (HMENU)ID_AUTOSTART, NULL, NULL);
            CreateWindowA("BUTTON", "Custom Background", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        10, 80, 140, 25, hwnd, (HMENU)ID_BACKGROUND_BROWSE, NULL, NULL);
            CreateWindowA("BUTTON", "Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        10, 115, 60, 25, hwnd, (HMENU)ID_EXIT, NULL, NULL);
            
            // 系統資源監控標籤
            hResourceLabel = CreateWindowA("STATIC", "System Resources: Loading...", 
                        WS_VISIBLE | WS_CHILD | SS_LEFT,
                        10, 150, 300, 20, hwnd, NULL, NULL, NULL);
            
            CheckDlgButton(hwnd, ID_AUTOSTART, IsAutoStartEnabled() ? BST_CHECKED : BST_UNCHECKED);
            InitSystemMonitor();
            break;
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_TOGGLE:
                    ToggleDesktop();
                    break;
                case ID_AUTOSTART:
                    SetAutoStart(IsDlgButtonChecked(hwnd, ID_AUTOSTART) == BST_CHECKED);
                    break;
                case ID_BACKGROUND_BROWSE: {
                    OPENFILENAMEA ofn;
                    memset(&ofn, 0, sizeof(ofn));
                    char szFile[MAX_PATH] = "";
                    
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "Bitmap Files\0*.bmp\0JPEG Files\0*.jpg;*.jpeg\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    
                    if (GetOpenFileNameA(&ofn)) {
                        strcpy(customBackgroundPath, szFile);
                        InvalidateRect(hwnd, NULL, TRUE); // 重繪視窗以顯示新背景
                    }
                    break;
                }
                case ID_SHOW_UI:
                    ShowWindow(hwnd, SW_SHOW);
                    SetForegroundWindow(hwnd);
                    break;
                case ID_EXIT:
                    RestoreDesktop();
                    RemoveSystemTray();
                    UnregisterGlobalHotkeys(hwnd);
                    CleanupSystemMonitor();
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
            else if (wParam == 3) {
                ShowWindow(hwnd, SW_SHOW);
                SetForegroundWindow(hwnd);
            }
            break;
            
        case WM_TIMER:
            if (wParam == TIMER_ID) {
                CheckMousePosition();
                
                // 更新系統資源顯示
                if (hResourceLabel) {
                    char resourceInfo[256];
                    GetSystemResourceInfo(resourceInfo, sizeof(resourceInfo));
                    SetWindowTextA(hResourceLabel, resourceInfo);
                }
            }
            break;
            
        case WM_TRAYICON:
            if (lParam == WM_LBUTTONDBLCLK) {
                ShowWindow(hwnd, IsWindowVisible(hwnd) ? SW_HIDE : SW_SHOW);
            } else if (lParam == WM_RBUTTONUP) {
                HMENU hMenu = CreatePopupMenu();
                AppendMenuA(hMenu, MF_STRING, ID_TOGGLE, IsDesktopHidden() ? "Show(Ctrl+Shift+D)" : "Hide(Ctrl+Shift+D)");
                AppendMenuA(hMenu, MF_STRING, ID_SHOW_UI, "Show UI(Crtl+Shift+U)");
                AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuA(hMenu, MF_STRING, ID_EXIT, "Exit(Crtl+Shift+Q)");
                
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
            CleanupSystemMonitor();
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
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    RegisterClassA(&wc);
    
    mainWindow = CreateWindowA("GhostDeskApp", "GhostDesk Control Panel", 
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                            CW_USEDEFAULT, CW_USEDEFAULT, 640, 360,
                            NULL, NULL, hInstance, NULL);
    
    // Multi-monitor support ready
    
    RegisterGlobalHotkeys(mainWindow);
    CreateSystemTray(mainWindow);
    SetTimer(mainWindow, TIMER_ID, 250, NULL); // Reduced frequency
    
    // Auto-hide desktop on startup
    ToggleDesktop();
    
    // 檢查命令列參數，決定是否顯示GUI
    bool showGUI = false;
    LPWSTR* szArglist;
    int nArgs;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    
    for (int i = 1; i < nArgs; i++) {
        if (wcscmp(szArglist[i], L"--show-gui") == 0 || wcscmp(szArglist[i], L"-g") == 0) {
            showGUI = true;
            break;
        }
    }
    LocalFree(szArglist);
    
    // 只有在明確要求時才顯示GUI，否則在後台運行
    if (showGUI) {
        ShowWindow(mainWindow, SW_SHOW);
    } else {
        ShowWindow(mainWindow, SW_HIDE);
    }
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}