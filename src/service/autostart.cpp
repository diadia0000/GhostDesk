#ifndef GHOSTDESK_EXPORTS
#define GHOSTDESK_EXPORTS
#endif
#include "../../include/ghostdesk_api.h"

BOOL SetAutoStart(BOOL enable) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, 
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
        0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) return FALSE;
    
    if (enable) {
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        result = RegSetValueExA(hKey, "GhostDesk", 0, REG_SZ, 
            (BYTE*)exePath, strlen(exePath) + 1);
    } else {
        result = RegDeleteValueA(hKey, "GhostDesk");
    }
    
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

BOOL IsAutoStartEnabled() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, 
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
        0, KEY_QUERY_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) return FALSE;
    
    DWORD type, size = 0;
    result = RegQueryValueExA(hKey, "GhostDesk", NULL, &type, NULL, &size);
    RegCloseKey(hKey);
    
    return (result == ERROR_SUCCESS && size > 0);
}