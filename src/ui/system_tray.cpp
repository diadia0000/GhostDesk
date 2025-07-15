#define GHOSTDESK_EXPORTS
#include "../../include/ghostdesk_api.h"
#include <shellapi.h>

static NOTIFYICONDATAA nid = {};

BOOL CreateSystemTray(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1003;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy(nid.szTip, "GhostDesk - U:UI D:Desktop Q:Exit");
    return Shell_NotifyIconA(NIM_ADD, &nid);
}

void RemoveSystemTray() {
    Shell_NotifyIconA(NIM_DELETE, &nid);
}