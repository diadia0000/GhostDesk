#  GhostDesk

讓桌面消失，只留下你跟你的專注。  
GhostDesk 是一款類似 Wallpaper Engine 的模組化桌面管理工具，  
開機自動隱藏桌面，提供小型控制面板，還你一片清靜 ✨

---

## 主要特色

- 🔹 **開機自動啟動** - 自動隱藏桌面，無需手動操作
- 🔹 **小型控制面板** - 類似 Wallpaper Engine 的簡潔介面
- 🔹 **模組化架構** - DLL 分離，易於維護和擴展
- 🔹 **系統匣整合** - 背景運行，右鍵快速控制
- 🔹 **自動註冊到 Windows** - 一鍵設定開機啟動
- 🔹 **版本控制友好** - dist 目錄管理發布版本
- 🔹 **極致效能優化** - 最小 CPU 和記憶體占用

---

## 專案架構

```
GhostDesk/
├── src/
│   ├── core/           # 核心功能 DLL
│   ├── ui/             # 介面模組 DLL  
│   ├── service/        # 服務模組 DLL
│   └── GhostDesk.cpp   # 主程式
├── include/            # API 頭文件
├── dist/               # 發布版本
└── build_modular.bat   # 編譯腳本
```

## 使用方式

### 編譯

請使用支援 Win32 API 的 C++ 編譯器（如 MinGW）

```bash
# 極致優化編譯（推薦）
build_optimized.bat

# 一般模組化編譯
build_modular.bat

# 手動優化編譯
g++ -shared -O3 -s -flto -DNDEBUG -DWIN32_LEAN_AND_MEAN -fno-exceptions src\core\*.cpp src\ui\*.cpp src\service\*.cpp -o dist\ghostdesk_core.dll -lshell32
g++ -O3 -s -flto -DNDEBUG -DWIN32_LEAN_AND_MEAN -fno-exceptions src\GhostDesk.cpp -o dist\GhostDesk.exe -mwindows -L.\dist -lghostdesk_core -lshell32
```

### 執行

```bash
# 從 dist 目錄執行
dist\GhostDesk.exe
```

程式會：
- 自動隱藏桌面（首次啟動）
- 在系統匣顯示圖示
- 控制面板預設隱藏，雙擊系統匣圖示顯示

---

## 操作方式

| 操作方式 | 功能 |
|----------|------|
| `Ctrl + Shift + D` | 顯示/隱藏 桌面圖示 與 Taskbar |
| `Ctrl + Shift + Q` | 退出程式 |
| `滑鼠移至螢幕底部` | 自動彈出 Taskbar |
| `雙擊系統匣圖示` | 顯示/隱藏控制視窗 |
| `右鍵系統匣圖示` | 開啟快速選單 |

---

## 開發環境

| 工具 | 說明 |
|------|------|
| OS | Windows 10/11 |
| 編譯器 | MinGW-w64 (推薦) |
| 架構 | 模組化 DLL + 主程式 |
| 語言 | C++17 |
| 技術 | Win32 API + DLL 分離 + 極致優化 |
| 效能 | < 1MB 記憶體，< 0.1% CPU |

---

## 開發中 / Todo

- [x] 滑鼠靠近底部自動顯示工作列
- [x] System Tray Icon 控制開關
- [x] 開機自動啟動（Windows Registry）
- [x] 模組化 DLL 架構
- [ ] 自定快捷鍵 + 設定介面
- [ ] Fade In/Out 動畫效果（用 `AnimateWindow()`）
- [ ] 顯示該App對系統占用量
---

## Side Project, Vibe On!

這不是一個要寫成巨獸的專案，  
而是一個讓我~~學習 C++ 與 Win32 API~~ 自嗨用的 Playground。  
寫一點，學一點，優雅地把想法做出來

---

> "有時候，你不是想關掉世界。你只是想把桌面藏起來而已。" — GhostDesk