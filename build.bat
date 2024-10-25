 @echo off
REM build.bat
echo Cleaning old build files...
rmdir /s /q build
mkdir build
pause
cd build
echo Configuring with Ninja...
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
pause
echo Building project...
cmake --build .
if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)
echo Build complete, press any key to run the executable...
pause
cd bin
echo Running Lancer-bot.exe...
pause
.\Lancer-bot.exe
echo Program finished, press any key to exit...
cd ..\..
pause
