#include <windows.h>

#define ID_TOGGLE 1001
#define ID_EXIT 1002

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateWindowA("BUTTON", "Toggle Desktop", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        50, 50, 150, 30, hwnd, (HMENU)ID_TOGGLE, NULL, NULL);
            CreateWindowA("BUTTON", "Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                        50, 100, 150, 30, hwnd, (HMENU)ID_EXIT, NULL, NULL);
            CreateWindowA("STATIC", "Ctrl+Shift+D: Toggle\nCtrl+Shift+Q: Exit", 
                        WS_VISIBLE | WS_CHILD | SS_LEFT,
                        50, 150, 200, 40, hwnd, NULL, NULL, NULL);
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TOGGLE) {
                // Send message to main app
                HWND mainApp = FindWindowA("GhostDesk", NULL);
                if (mainApp) PostMessage(mainApp, WM_HOTKEY, 1, 0);
            } else if (LOWORD(wParam) == ID_EXIT) {
                // Exit main app first
                HWND mainApp = FindWindowA("GhostDesk", NULL);
                if (mainApp) PostMessage(mainApp, WM_HOTKEY, 2, 0);
                PostQuitMessage(0);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Check if main app is running, if not, start it
    HWND mainApp = FindWindowA("GhostDesk", NULL);
    if (!mainApp) {
        ShellExecuteA(NULL, "open", "ghostdesk.exe", NULL, NULL, SW_HIDE);
        Sleep(500); // Wait for main app to start
    }
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GhostDeskUI";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);
    
    HWND hwnd = CreateWindowA("GhostDeskUI", "GhostDesk Control", 
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                            CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
                            NULL, NULL, hInstance, NULL);
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}