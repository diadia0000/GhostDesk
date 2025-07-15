#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"
#include <windows.h>

static bool isHidden = false;
static HWND desktopIcons = NULL;
static HWND taskbar = NULL;
HWND taskbars[10] = {0};
int taskbarCount = 0;
static bool taskbarVisible[10] = {false}; // Per-monitor visibility state
static bool animating[10] = {false}; // Per-monitor animation state

// 假工作列系統
static const char* FAKE_TASKBAR_CLASS = "GhostDesk_FakeTaskbar";
static HWND fakeTaskbars[10] = {0};
static HDC taskbarDCs[10] = {0};
static HBITMAP taskbarBitmaps[10] = {0};
static int taskbarWidths[10] = {0};
static int taskbarHeights[10] = {0};

// 假工作列視窗處理函數
LRESULT CALLBACK FakeTaskbarProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // 找到對應的假工作列索引
    int index = -1;
    for (int i = 0; i < taskbarCount; i++) {
        if (fakeTaskbars[i] == hwnd) {
            index = i;
            break;
        }
    }
    
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (index >= 0 && taskbarDCs[index] && taskbarBitmaps[index]) {
                // 繪製工作列截圖
                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, taskbarBitmaps[index]);
                BitBlt(hdc, 0, 0, taskbarWidths[index], taskbarHeights[index], 
                       taskbarDCs[index], 0, 0, SRCCOPY);
                SelectObject(memDC, oldBitmap);
                DeleteDC(memDC);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 初始化假工作列
void InitFakeTaskbar() {
    static bool initialized = false;
    if (initialized) return;
    
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = FakeTaskbarProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = FAKE_TASKBAR_CLASS;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    RegisterClassA(&wc);
    initialized = true;
    
    // 為每個工作列創建截圖資源
    for (int i = 0; i < taskbarCount; i++) {
        RECT rect;
        GetWindowRect(taskbars[i], &rect);
        taskbarWidths[i] = rect.right - rect.left;
        taskbarHeights[i] = rect.bottom - rect.top;
        
        // 創建相容的 DC 和位圖
        HDC screenDC = GetDC(NULL);
        taskbarDCs[i] = CreateCompatibleDC(screenDC);
        taskbarBitmaps[i] = CreateCompatibleBitmap(screenDC, taskbarWidths[i], taskbarHeights[i]);
        SelectObject(taskbarDCs[i], taskbarBitmaps[i]);
        ReleaseDC(NULL, screenDC);
    }
}

// 清理假工作列資源
void CleanupFakeTaskbar() {
    for (int i = 0; i < taskbarCount; i++) {
        if (fakeTaskbars[i]) {
            DestroyWindow(fakeTaskbars[i]);
            fakeTaskbars[i] = NULL;
        }
        
        if (taskbarBitmaps[i]) {
            DeleteObject(taskbarBitmaps[i]);
            taskbarBitmaps[i] = NULL;
        }
        
        if (taskbarDCs[i]) {
            DeleteDC(taskbarDCs[i]);
            taskbarDCs[i] = NULL;
        }
    }
}

// 截取工作列畫面
void CaptureTaskbarImage(int monitorIndex) {
    if (monitorIndex < 0 || monitorIndex >= taskbarCount) return;
    
    RECT rect;
    GetWindowRect(taskbars[monitorIndex], &rect);
    
    // 確保工作列可見並更新
    ShowWindow(taskbars[monitorIndex], SW_SHOW);
    UpdateWindow(taskbars[monitorIndex]);
    Sleep(50);
    
    // 截取工作列畫面
    HDC screenDC = GetDC(NULL);
    if (taskbarDCs[monitorIndex] && taskbarBitmaps[monitorIndex]) {
        BitBlt(taskbarDCs[monitorIndex], 0, 0, taskbarWidths[monitorIndex], taskbarHeights[monitorIndex], 
               screenDC, rect.left, rect.top, SRCCOPY);
    }
    ReleaseDC(NULL, screenDC);
}

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
    if (!isHidden) {
        // 隱藏前先初始化和截圖
        InitFakeTaskbar();
        for (int i = 0; i < taskbarCount; i++) {
            CaptureTaskbarImage(i);
        }
    }
    
    int cmd = isHidden ? SW_SHOW : SW_HIDE;
    for (int i = 0; i < taskbarCount; i++) {
        ShowWindow(taskbars[i], cmd);
        taskbarVisible[i] = false;
        animating[i] = false;
    }
    ShowWindow(desktopIcons, cmd);
    isHidden = !isHidden;
    
    // 如果切換為顯示模式，清理假工作列
    if (!isHidden) {
        CleanupFakeTaskbar();
    }
}

BOOL IsDesktopHidden() {
    return isHidden;
}

void RestoreDesktop() {
    if (isHidden) {
        for (int i = 0; i < taskbarCount; i++) {
            ShowWindow(taskbars[i], SW_SHOW);
            taskbarVisible[i] = false;
            animating[i] = false;
            SetWindowLongA(taskbars[i], GWL_EXSTYLE, GetWindowLongA(taskbars[i], GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
        ShowWindow(desktopIcons, SW_SHOW);
        isHidden = false;
        
        // 清理假工作列資源
        CleanupFakeTaskbar();
    }
}

void ShowTaskbarAnimated(int monitorIndex) {
    if (isHidden && monitorIndex >= 0 && monitorIndex < taskbarCount && 
        !taskbarVisible[monitorIndex] && !animating[monitorIndex]) {
        
        animating[monitorIndex] = true;
        HWND hwnd = taskbars[monitorIndex];
        
        // 獲取工作列位置和大小
        RECT rect;
        GetWindowRect(hwnd, &rect);
        int width = taskbarWidths[monitorIndex];
        int height = taskbarHeights[monitorIndex];
        
        // 創建假工作列
        fakeTaskbars[monitorIndex] = CreateWindowExA(
            WS_EX_LAYERED | WS_EX_TOPMOST,
            FAKE_TASKBAR_CLASS, NULL, WS_POPUP,
            rect.left, rect.bottom, width, height,
            NULL, NULL, GetModuleHandle(NULL), NULL
        );
        
        // 設置透明度
        SetLayeredWindowAttributes(fakeTaskbars[monitorIndex], 0, 255, LWA_ALPHA);
        ShowWindow(fakeTaskbars[monitorIndex], SW_SHOW);
        
        // 執行滑動動畫
        for (int step = 0; step <= 10; step++) {
            int y = rect.bottom - (height * step / 10);
            SetWindowPos(fakeTaskbars[monitorIndex], HWND_TOPMOST, rect.left, y, width, height, SWP_NOACTIVATE);
            Sleep(15);
        }
        
        // 動畫結束後清理假工作列
        DestroyWindow(fakeTaskbars[monitorIndex]);
        fakeTaskbars[monitorIndex] = NULL;
        
        // 顯示真實工作列
        ShowWindow(hwnd, SW_SHOW);
        
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
        
        // 重新截取當前工作列畫面
        CaptureTaskbarImage(monitorIndex);
        
        // 獲取工作列位置和大小
        RECT rect;
        GetWindowRect(hwnd, &rect);
        int width = taskbarWidths[monitorIndex];
        int height = taskbarHeights[monitorIndex];
        
        // 創建假工作列
        fakeTaskbars[monitorIndex] = CreateWindowExA(
            WS_EX_LAYERED | WS_EX_TOPMOST,
            FAKE_TASKBAR_CLASS, NULL, WS_POPUP,
            rect.left, rect.top, width, height,
            NULL, NULL, GetModuleHandle(NULL), NULL
        );
        
        // 設置透明度
        SetLayeredWindowAttributes(fakeTaskbars[monitorIndex], 0, 255, LWA_ALPHA);
        ShowWindow(fakeTaskbars[monitorIndex], SW_SHOW);
        
        // 隱藏真實工作列
        ShowWindow(hwnd, SW_HIDE);
        
        // 執行滑動動畫
        for (int step = 0; step <= 10; step++) {
            int y = rect.top + (height * step / 10);
            SetWindowPos(fakeTaskbars[monitorIndex], HWND_TOPMOST, rect.left, y, width, height, SWP_NOACTIVATE);
            Sleep(15);
        }
        
        // 清理假工作列
        DestroyWindow(fakeTaskbars[monitorIndex]);
        fakeTaskbars[monitorIndex] = NULL;
        
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