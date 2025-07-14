@echo off
g++ main.cpp -o ghostdesk.exe -mwindows -static
g++ ui.cpp -o ghostdesk_ui.exe -mwindows -static -lshell32
g++ launcher.cpp -o ghostdesk_launcher.exe -mwindows -static -lshell32
echo Build complete. Run: ghostdesk_launcher.exe
pause