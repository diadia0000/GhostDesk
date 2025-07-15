#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"

#define HOTKEY_TOGGLE 1
#define HOTKEY_EXIT 2
#define HOTKEY_SHOW_UI 3

BOOL RegisterGlobalHotkeys(HWND hwnd) {
    BOOL result = TRUE;
    result &= RegisterHotKey(hwnd, HOTKEY_TOGGLE, MOD_CONTROL | MOD_SHIFT, 'D');
    result &= RegisterHotKey(hwnd, HOTKEY_EXIT, MOD_CONTROL | MOD_SHIFT, 'Q');
    result &= RegisterHotKey(hwnd, HOTKEY_SHOW_UI, MOD_CONTROL | MOD_SHIFT, 'U');
    return result;
}

void UnregisterGlobalHotkeys(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_TOGGLE);
    UnregisterHotKey(hwnd, HOTKEY_EXIT);
    UnregisterHotKey(hwnd, HOTKEY_SHOW_UI);
}