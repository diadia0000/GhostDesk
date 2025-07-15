@echo off
echo Building and running GhostDesk tests...
echo.

g++ -std=c++17 -O2 -Wall test_runner.cpp -o test_runner.exe -luser32 -lshell32

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Running tests...
    echo.
    test_runner.exe
) else (
    echo Build failed!
    pause
)