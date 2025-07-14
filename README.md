#  GhostDesk

讓桌面消失，只留下你跟你的專注。  
GhostDesk 是一款超低資源、可隱藏 Windows 桌面與工作列的極簡工具，  
按下熱鍵，一鍵清空視覺干擾，還你一片清靜 ✨

---

## 主要特色

- 🔹 隱藏桌面圖示 & 任務列（Taskbar）
- 🔹 按下熱鍵即可還原或再次隱藏
- 🔹 底部滑鼠懸停時可自動彈出工具列（開發中）
- 🔹 極致輕量，不啟用 UI，不閃視窗
- 🔹 支援 VSCode / C++ / MinGW 開發

---

## 使用方式

### 編譯

請使用支援 Win32 API 的 C++ 編譯器（如 MinGW）

```bash
g++ main.cpp -o ghostdesk.exe -mwindows
```

### 執行

```bash
./ghostdesk.exe
```

程式會靜默運行於背景，不會開啟視窗。  
熱鍵已註冊為：`Ctrl + Shift + D` Toggle 顯示 / 隱藏

---

## 快捷鍵操作

| 快捷鍵 | 功能 |
|--------|------|
| `Ctrl + Shift + D` | 顯示/隱藏 桌面圖示 與 Taskbar |
| `Mouse to bottom edge` | 自動彈出 Taskbar（開發中） |

---

## 開發環境

| 工具 | 說明 |
|------|------|
| OS | Windows 10/11 |
| 編譯器 | MinGW-w64 or MSYS2 (推薦) |
| 編輯器 | VSCode |
| 語言 | C++17 / C++20 |
| 技術 | 原生 Win32 API（無 GUI 框架） |

---

## 開發中 / Todo

- [ ] 滑鼠靠近底部自動顯示工作列
- [ ] System Tray Icon 控制開關
- [ ] 開機自動啟動（Windows Registry）
- [ ] 自定快捷鍵 + 設定介面
- [ ] Fade In/Out 動畫效果（用 `AnimateWindow()`）

---

## Side Project, Vibe On!

這不是一個要寫成巨獸的專案，  
而是一個讓我學習 C++ 與 Win32 API 的 Playground。  
寫一點，學一點，優雅地把想法做出來

---

> “有時候，你不是想關掉世界。你只是想把桌面藏起來而已。” — GhostDesk
