#ifndef GHOSTDESK_EXPORTS
#define GHOSTDESK_EXPORTS
#endif
#include "../../include/ghostdesk_api.h"
#include <shellapi.h>

static NOTIFYICONDATAA nid = {};

BOOL CreateSystemTray(HWND hwnd) {
    nid.cbSize = NOTIFYICONDATA_V2_SIZE; // 使用較小的結構大小
    nid.hWnd = hwnd;
    nid.uID = 1003;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
    if (!nid.hIcon) nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy(nid.szTip, "GhostDesk"); // 簡化提示文字
    return Shell_NotifyIconA(NIM_ADD, &nid);
}

void RemoveSystemTray() {
    Shell_NotifyIconA(NIM_DELETE, &nid);
}