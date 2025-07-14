@echo off
echo Building GhostDesk optimized version...

if not exist dist mkdir dist

echo Compiling core modules (optimized)...
g++ -shared -O3 -s -flto -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all -DNDEBUG -DWIN32_LEAN_AND_MEAN -fno-exceptions -fno-rtti src\core\desktop_control.cpp src\core\hotkey_manager.cpp src\ui\system_tray.cpp src\service\autostart.cpp -o dist\ghostdesk_core.dll -lshell32

echo Compiling main program (optimized)...
g++ -O3 -s -flto -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all -DNDEBUG -DWIN32_LEAN_AND_MEAN -fno-exceptions -fno-rtti src\GhostDesk.cpp -o dist\GhostDesk.exe -mwindows -L.\dist -lghostdesk_core -lshell32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo Output: dist\GhostDesk.exe
    echo Optimizations:
    echo    - LTO optimization
    echo    - No exceptions/RTTI
    echo    - Minimal Win32 API
    echo    - Reduced memory allocation
    echo    - Lower CPU usage
) else (
    echo Build failed!
)

pause