@echo off
setlocal enabledelayedexpansion

echo ========================================
echo          GhostDesk Build System
echo ========================================

cd /d "%~dp0"

REM 創建必要目錄
if not exist dist mkdir dist
if not exist resources mkdir resources

REM 顯示選單
:menu
echo.
echo Please select build type:
echo [1] Debug Build     - Development with debug symbols
echo [2] Release Build   - Standard optimized build  
echo [3] Optimized Build - Maximum performance build
echo [4] All Builds      - Build all versions
echo [5] Clean Only      - Clean build files
echo [0] Exit
echo.
set /p choice="Enter your choice (0-5): "

if "%choice%"=="1" goto debug_build
if "%choice%"=="2" goto release_build
if "%choice%"=="3" goto optimized_build
if "%choice%"=="4" goto all_builds
if "%choice%"=="5" goto clean_only
if "%choice%"=="0" goto exit
echo Invalid choice. Please try again.
goto menu

:clean_only
echo.
echo [CLEAN] Cleaning build files...
taskkill /f /im GhostDesk.exe 2>nul
del /q dist\*.dll dist\*.exe 2>nul
del /q resources\*.o 2>nul
echo Clean completed.
goto menu

:debug_build
echo.
echo ========================================
echo     Building GhostDesk Debug Version
echo ========================================
call :clean_files
call :compile_dll debug
if !ERRORLEVEL! NEQ 0 goto build_failed
call :compile_resources
call :compile_exe debug
if !ERRORLEVEL! NEQ 0 goto build_failed
call :show_debug_info
goto menu

:release_build
echo.
echo ========================================
echo    Building GhostDesk Release Version
echo ========================================
call :clean_files
call :compile_dll release
if !ERRORLEVEL! NEQ 0 goto build_failed
call :compile_resources
call :compile_exe release
if !ERRORLEVEL! NEQ 0 goto build_failed
call :show_release_info
goto menu

:optimized_build
echo.
echo ========================================
echo   Building GhostDesk Optimized Version
echo ========================================
call :clean_files
call :compile_dll optimized
if !ERRORLEVEL! NEQ 0 goto build_failed
call :compile_resources
call :compile_exe optimized
if !ERRORLEVEL! NEQ 0 goto build_failed
call :show_optimized_info
goto menu

:all_builds
echo.
echo ========================================
echo      Building All GhostDesk Versions
echo ========================================

echo.
echo [1/3] Building Debug Version...
call :clean_files
call :compile_dll debug
if !ERRORLEVEL! NEQ 0 goto build_failed
call :compile_resources
call :compile_exe debug
if !ERRORLEVEL! NEQ 0 goto build_failed
copy /y "dist\GhostDesk.exe" "dist\GhostDesk_debug.exe" >nul
copy /y "dist\ghostdesk_core.dll" "dist\ghostdesk_core_debug.dll" >nul

echo.
echo [2/3] Building Release Version...
call :clean_files
call :compile_dll release
if !ERRORLEVEL! NEQ 0 goto build_failed
call :compile_resources
call :compile_exe release
if !ERRORLEVEL! NEQ 0 goto build_failed
copy /y "dist\GhostDesk.exe" "dist\GhostDesk_release.exe" >nul
copy /y "dist\ghostdesk_core.dll" "dist\ghostdesk_core_release.dll" >nul

echo.
echo [3/3] Building Optimized Version...
call :clean_files
call :compile_dll optimized
if !ERRORLEVEL! NEQ 0 goto build_failed
call :compile_resources
call :compile_exe optimized
if !ERRORLEVEL! NEQ 0 goto build_failed
copy /y "dist\GhostDesk.exe" "dist\GhostDesk_optimized.exe" >nul
copy /y "dist\ghostdesk_core.dll" "dist\ghostdesk_core_optimized.dll" >nul

call :show_all_builds_info
goto menu

REM ========================================
REM           HELPER FUNCTIONS
REM ========================================

:clean_files
echo [CLEAN] Cleaning old files...
taskkill /f /im GhostDesk.exe 2>nul
del /q dist\*.dll dist\*.exe 2>nul
del /q resources\*.o 2>nul
exit /b 0

:compile_dll
set build_type=%1
echo [DLL] Compiling core modules (%build_type%)...
echo   - Desktop Control (src\core\desktop_control.cpp)
echo   - Hotkey Manager (src\core\hotkey_manager.cpp)
echo   - System Monitor (src\core\system_monitor.cpp)
echo   - System Tray (src\ui\system_tray.cpp)
echo   - Auto Start (src\service\autostart.cpp)

if "%build_type%"=="debug" (
    g++ -shared -g -O0 -DDEBUG -DGHOSTDESK_EXPORTS -DWIN32_LEAN_AND_MEAN ^
        -Wall -Wno-missing-field-initializers -I"%~dp0include" ^
        "%~dp0src\core\desktop_control.cpp" ^
        "%~dp0src\core\hotkey_manager.cpp" ^
        "%~dp0src\core\system_monitor.cpp" ^
        "%~dp0src\ui\system_tray.cpp" ^
        "%~dp0src\service\autostart.cpp" ^
        -o "%~dp0dist\ghostdesk_core.dll" ^
        -lshell32 -lgdi32 -lpsapi -lpdh -luser32
) else if "%build_type%"=="release" (
    g++ -shared -O2 -s -DGHOSTDESK_EXPORTS -DNDEBUG -DWIN32_LEAN_AND_MEAN ^
        -I"%~dp0include" ^
        "%~dp0src\core\desktop_control.cpp" ^
        "%~dp0src\core\hotkey_manager.cpp" ^
        "%~dp0src\core\system_monitor.cpp" ^
        "%~dp0src\ui\system_tray.cpp" ^
        "%~dp0src\service\autostart.cpp" ^
        -o "%~dp0dist\ghostdesk_core.dll" ^
        -lshell32 -lgdi32 -lpsapi -lpdh -luser32
) else if "%build_type%"=="optimized" (
    g++ -shared -O3 -s -flto -ffunction-sections -fdata-sections ^
        -Wl,--gc-sections -DGHOSTDESK_EXPORTS -DNDEBUG -DWIN32_LEAN_AND_MEAN ^
        -fno-exceptions -fno-rtti -ffast-math -march=native ^
        -I"%~dp0include" ^
        "%~dp0src\core\desktop_control.cpp" ^
        "%~dp0src\core\hotkey_manager.cpp" ^
        "%~dp0src\core\system_monitor.cpp" ^
        "%~dp0src\ui\system_tray.cpp" ^
        "%~dp0src\service\autostart.cpp" ^
        -o "%~dp0dist\ghostdesk_core.dll" ^
        -lshell32 -lgdi32 -lpsapi -lpdh -luser32
)

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: DLL compilation failed!
    exit /b 1
)
exit /b 0

:compile_resources
echo [RC] Compiling resources...
if exist "%~dp0resources\app.rc" (
    windres "%~dp0resources\app.rc" -o "%~dp0resources\app.o"
    if !ERRORLEVEL! NEQ 0 (
        echo WARNING: Resource compilation failed, continuing without resources...
        set RESOURCE_OBJ=
    ) else (
        set RESOURCE_OBJ="%~dp0resources\app.o"
    )
) else (
    echo WARNING: app.rc not found, building without resources...
    set RESOURCE_OBJ=
)
exit /b 0

:compile_exe
set build_type=%1
echo [EXE] Compiling main program (%build_type%)...
echo   - Main Application (src\GhostDesk.cpp)

if "%build_type%"=="debug" (
    g++ -g -O0 -DDEBUG -DWIN32_LEAN_AND_MEAN ^
        -Wall -Wno-missing-field-initializers -I"%~dp0include" ^
        "%~dp0src\GhostDesk.cpp" !RESOURCE_OBJ! ^
        -o "%~dp0dist\GhostDesk.exe" ^
        -mconsole -L"%~dp0dist" -lghostdesk_core ^
        -lshell32 -lcomdlg32 -luser32 -lgdi32
) else if "%build_type%"=="release" (
    g++ -O2 -s -DNDEBUG -DWIN32_LEAN_AND_MEAN ^
        -I"%~dp0include" ^
        "%~dp0src\GhostDesk.cpp" !RESOURCE_OBJ! ^
        -o "%~dp0dist\GhostDesk.exe" ^
        -mwindows -L"%~dp0dist" -lghostdesk_core ^
        -lshell32 -lcomdlg32 -luser32 -lgdi32
) else if "%build_type%"=="optimized" (
    g++ -O3 -s -flto -ffunction-sections -fdata-sections ^
        -Wl,--gc-sections,--strip-all -DNDEBUG -DWIN32_LEAN_AND_MEAN ^
        -fno-exceptions -fno-rtti -ffast-math -march=native ^
        -fomit-frame-pointer -fmerge-all-constants ^
        -I"%~dp0include" ^
        "%~dp0src\GhostDesk.cpp" !RESOURCE_OBJ! ^
        -o "%~dp0dist\GhostDesk.exe" ^
        -mwindows -L"%~dp0dist" -lghostdesk_core ^
        -lshell32 -lcomdlg32 -luser32 -lgdi32
)

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Main program compilation failed!
    exit /b 1
)
exit /b 0

:show_debug_info
echo.
echo ========================================
echo        Debug Build Completed!
echo ========================================
echo   Output: dist\GhostDesk.exe
echo   Library: dist\ghostdesk_core.dll
echo.
echo Debug Features:
echo   ✓ Debug symbols included
echo   ✓ Console output enabled
echo   ✓ Extended warnings
echo   ✓ All assertions enabled
echo.
goto show_usage

:show_release_info
echo.
echo ========================================
echo       Release Build Completed!
echo ========================================
echo   Output: dist\GhostDesk.exe
echo   Library: dist\ghostdesk_core.dll
echo.
echo Release Features:
echo   ✓ Standard optimization (O2)
echo   ✓ Stripped binaries
echo   ✓ Windows subsystem
echo   ✓ Production ready
echo.
goto show_usage

:show_optimized_info
echo.
echo ========================================
echo      Optimized Build Completed!
echo ========================================
echo   Output: dist\GhostDesk.exe
echo   Library: dist\ghostdesk_core.dll
echo.
echo Optimizations Applied:
echo   ✓ Maximum optimization (O3)
echo   ✓ Link-time optimization (LTO)
echo   ✓ Dead code elimination
echo   ✓ Native CPU optimization
echo   ✓ No exceptions/RTTI
echo   ✓ Fast math operations
echo.
echo Performance Target:
echo   • Memory Usage: < 1MB
echo   • CPU Usage: < 0.1%
echo   • Binary Size: Minimized
echo.
goto show_usage

:show_all_builds_info
echo.
echo ========================================
echo       All Builds Completed!
echo ========================================
echo Available executables in dist\:
dir dist\*.exe /b 2>nul
echo.
echo Available libraries in dist\:
dir dist\*.dll /b 2>nul
echo.
goto show_usage

:show_usage
echo Usage:
echo   dist\GhostDesk.exe              (Background mode)
echo   dist\GhostDesk.exe --show-gui   (With GUI)
echo.
echo Features Included:
echo   ✓ Multi-monitor desktop hiding
echo   ✓ Animated taskbar show/hide
echo   ✓ System tray integration
echo   ✓ Global hotkeys support
echo   ✓ Auto-start functionality
echo   ✓ System resource monitoring
echo   ✓ Custom GUI backgrounds
echo   ✓ Background execution mode
echo   ✓ Modular DLL architecture
echo.
exit /b 0

:build_failed
echo.
echo ========================================
echo           BUILD FAILED!
echo ========================================
echo Please check the error messages above.
echo.
pause
goto menu

:exit
echo.
echo Goodbye!
exit /b 0