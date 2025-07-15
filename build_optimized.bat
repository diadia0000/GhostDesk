@echo off
echo Building GhostDesk optimized version...

cd /d "%~dp0"

if not exist dist mkdir dist

echo Cleaning old files...
taskkill /f /im GhostDesk.exe 2>nul
del /q dist\*.dll dist\*.exe 2>nul

echo Compiling core modules (optimized)...
g++ -shared -O3 -s -ffunction-sections -fdata-sections -Wl,--gc-sections -DNDEBUG -DWIN32_LEAN_AND_MEAN -fno-exceptions -fno-rtti "%~dp0src\core\desktop_control.cpp" "%~dp0src\core\hotkey_manager.cpp" "%~dp0src\ui\system_tray.cpp" "%~dp0src\service\autostart.cpp" -o "%~dp0dist\ghostdesk_core.dll" -lshell32

if %ERRORLEVEL% NEQ 0 (
    echo DLL compilation failed!
    pause
    exit /b 1
)

echo Compiling main program (optimized)...
g++ -O3 -s -ffunction-sections -fdata-sections -Wl,--gc-sections -DNDEBUG -DWIN32_LEAN_AND_MEAN -fno-exceptions -fno-rtti "%~dp0src\GhostDesk.cpp" -o "%~dp0dist\GhostDesk.exe" -mwindows -L"%~dp0dist" -lghostdesk_core -lshell32

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo Output: dist\GhostDesk.exe
    echo Optimizations:
    echo    - O3 optimization
    echo    - No exceptions/RTTI
    echo    - Minimal Win32 API
    echo    - Function/data sections
    echo    - Dead code elimination
) else (
    echo Build failed!
)

pause