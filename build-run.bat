@echo off
REM build-run.bat

echo Starting build process...

cd build

echo Building project...
cmake --build . --config Debug


if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build complete, running executable...
echo.

REM Navigate to where your executable is and run it
cd bin
.\Lancer-bot.exe

cd ....



echo. 
pause

