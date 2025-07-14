#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // Start main app in background
    ShellExecuteA(NULL, "open", "ghostdesk.exe", NULL, NULL, SW_HIDE);
    Sleep(500);
    
    // Start UI
    ShellExecuteA(NULL, "open", "ghostdesk_ui.exe", NULL, NULL, SW_SHOW);
    
    return 0;
}