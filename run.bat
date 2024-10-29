 @echo off
REM run.bat

cd build
cd bin

if not exist "build\bin" (
    echo Error: build/bin directory not found
    exit /b 1
)

echo Running Project
echo. 

.\Lancer-bot.exe


if not exist ".\Lancer-bot.exe" (
    echo Error: Lancer-bot.exe not found
    exit /b 1
)


echo.
echo Finished running Project

