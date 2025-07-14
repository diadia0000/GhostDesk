@echo off
echo Building GhostDesk modular version...

REM Create dist directory
if not exist dist mkdir dist

REM Compile core DLL
echo Compiling core modules...
g++ -shared -O3 -s -ffunction-sections -fdata-sections -Wl,--gc-sections -DNDEBUG src\core\desktop_control.cpp src\core\hotkey_manager.cpp src\ui\system_tray.cpp src\service\autostart.cpp -o dist\ghostdesk_core.dll -lshell32

REM Compile main program
echo Compiling main program...
g++ -O3 -s -ffunction-sections -fdata-sections -Wl,--gc-sections -DNDEBUG src\GhostDesk.cpp -o dist\GhostDesk.exe -mwindows -L.\dist -lghostdesk_core -lshell32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo Output directory: dist\
    echo Executable: dist\GhostDesk.exe
    echo Core library: dist\ghostdesk_core.dll
    echo.
    echo Features:
    echo - Auto-start desktop hiding
    echo - Small control panel
    echo - System tray integration
    echo - Modular architecture
) else (
    echo Build failed!
)

pause