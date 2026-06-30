@echo off
echo Compiling Snake Game...

g++ main.cpp -o SnakeGame.exe ^
  -I"E:\SFML-SEH\include" ^
  -L"E:\SFML-SEH\lib" ^
  -lsfml-graphics -lsfml-window -lsfml-system ^
  -mwindows

if %ERRORLEVEL% == 0 (
    echo.
    echo SUCCESS! Running Snake Game...
    start SnakeGame.exe
) else (
    echo BUILD FAILED. Check errors above.
    pause
)
