@echo off
echo 正在編譯 GhostDesk 模組化版本...

REM 創建 dist 目錄
if not exist dist mkdir dist

REM 編譯核心 DLL
echo 編譯核心模組...
g++ -shared src\core\desktop_control.cpp src\core\hotkey_manager.cpp src\ui\system_tray.cpp src\service\autostart.cpp -o dist\ghostdesk_core.dll -lshell32

REM 編譯主程式
echo 編譯主程式...
g++ src\GhostDesk.cpp -o dist\GhostDesk.exe -mwindows -L.\dist -lghostdesk_core -lshell32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ 編譯成功！
    echo 📁 輸出目錄: dist\
    echo 🚀 執行檔: dist\GhostDesk.exe
    echo 📚 核心庫: dist\ghostdesk_core.dll
    echo.
    echo 功能特色:
    echo - 開機自動啟動桌面隱藏
    echo - 小型控制面板
    echo - 系統匣整合
    echo - 模組化架構
) else (
    echo ❌ 編譯失敗！
)

pause