@echo off
echo Building GhostDesk modular version...

cd /d "%~dp0"

if not exist dist mkdir dist

echo Cleaning old files...
taskkill /f /im GhostDesk.exe 2>nul
del /q dist\*.dll dist\*.exe 2>nul

echo Compiling core modules...
g++ -shared -O2 -s -DNDEBUG -DWIN32_LEAN_AND_MEAN "%~dp0src\core\desktop_control.cpp" "%~dp0src\core\hotkey_manager.cpp" "%~dp0src\ui\system_tray.cpp" "%~dp0src\service\autostart.cpp" -o "%~dp0dist\ghostdesk_core.dll" -lshell32

if %ERRORLEVEL% NEQ 0 (
    echo DLL compilation failed!
    pause
    exit /b 1
)

echo Compiling main program...
g++ -O2 -s -DNDEBUG -DWIN32_LEAN_AND_MEAN "%~dp0src\GhostDesk.cpp" -o "%~dp0dist\GhostDesk.exe" -mwindows -L"%~dp0dist" -lghostdesk_core -lshell32

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