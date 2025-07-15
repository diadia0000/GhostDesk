#pragma once
#include <windows.h>

#ifdef GHOSTDESK_EXPORTS
#define GHOSTDESK_API __declspec(dllexport)
#else
#define GHOSTDESK_API __declspec(dllimport)
#endif

extern "C" {
    // Desktop Control
    GHOSTDESK_API BOOL InitDesktopControl();
    GHOSTDESK_API void ToggleDesktop();
    GHOSTDESK_API BOOL IsDesktopHidden();
    GHOSTDESK_API void RestoreDesktop();
    GHOSTDESK_API void ShowTaskbarAnimated(int monitorIndex);
    GHOSTDESK_API void HideTaskbarAnimated(int monitorIndex);
    GHOSTDESK_API void ShowAllTaskbarsAnimated();
    GHOSTDESK_API void HideAllTaskbarsAnimated();
    
    // Hotkey Management
    GHOSTDESK_API BOOL RegisterGlobalHotkeys(HWND hwnd);
    GHOSTDESK_API void UnregisterGlobalHotkeys(HWND hwnd);
    
    // System Tray
    GHOSTDESK_API BOOL CreateSystemTray(HWND hwnd);
    GHOSTDESK_API void RemoveSystemTray();
    
    // Auto-start
    GHOSTDESK_API BOOL SetAutoStart(BOOL enable);
    GHOSTDESK_API BOOL IsAutoStartEnabled();
    
    // Taskbar access
    GHOSTDESK_API extern HWND taskbars[10];
    GHOSTDESK_API extern int taskbarCount;
}