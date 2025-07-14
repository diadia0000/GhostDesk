#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"
#include <shellapi.h>

static NOTIFYICONDATA nid = {};

BOOL CreateSystemTray(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1003;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy(nid.szTip, "GhostDesk - Ctrl+Shift+D");
    return Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveSystemTray() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}