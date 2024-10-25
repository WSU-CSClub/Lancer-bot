# CMake Project Build Guide

This guide explains how to build and run the Lancer-Bot project using CMake across different operating systems.

## Prerequisites

- CMake (version 3.10 or higher)
- A C++ compiler:
  - Windows: MSVC or MinGW
  - Linux: GCC/G++
  - macOS: Clang
- For Windows: 
  - Ninja build system
  - Visual Studio Build Tools

## Project Structure
```
project/
├── CMakeLists.txt
├── src/
│   └── main.cpp
├── engine/
│   └── *.cpp
├── scripts/
│   ├── build_run.bat    (Windows build & run)
│   ├── run.bat          (Windows run only)
│   └── build.sh         (Linux/macOS)
└── README.md
```

## Build Instructions by Operating System

### Windows

#### Using build_run.bat (Recommended)
1. Open Command Prompt in the project directory
2. Run the build and execute script:
```batch
scripts\build_run.bat
```
This script will:
- Build the project using existing build files
- Run the executable if build succeeds
- Return to the original directory

#### Using build.bat (First time/Clean build)
1. Open Command Prompt in the project directory
2. Run the full build script:
```batch
scripts\build.bat
```
This script will:
- Clean old build files
- Create new build directory
- Configure CMake with Ninja
- Build the project
- Run the executable
- Guide you through each step with pauses

### Linux

1. Make the build script executable:
```bash
chmod +x scripts/build.sh
```

2. Run the script with options:
- `./scripts/build.sh clean` - Clean build directory
- `./scripts/build.sh build` - Only build the project
- `./scripts/build.sh run` - Only run the executable
- `./scripts/build.sh` - Build and run (default)

### macOS

Execute these commands in terminal:
```bash
mkdir build
cd build
cmake ..     # Only needed first time
make
cd bin
./Lancer-Bot
```

## Manual Build Process

### Windows (using Ninja)
```batch
mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
cmake --build .
cd bin
.\Lancer-bot.exe
```

### Linux/macOS
```bash
mkdir build
cd build
cmake ..
make
cd bin
./Lancer-Bot
```

## Troubleshooting

### Common Issues

1. **CMake not found**
   - Windows: Add CMake to System PATH
   - Linux: `sudo apt install cmake`
   - macOS: `brew install cmake`

2. **Ninja not found on Windows**
   - Install Ninja: `choco install ninja` (using Chocolatey)
   - Or download from Ninja's GitHub releases
   - Add to System PATH

3. **Compiler not found**
   - Windows: Install Visual Studio Build Tools or MinGW
   - Linux: `sudo apt install build-essential`
   - macOS: Install Xcode Command Line Tools

4. **Permission Issues**
   - Windows: Run Command Prompt as Administrator
   - Linux/macOS: Use `sudo` if needed for installations

### Path Issues

- Use forward slashes (/) in CMake files
- Avoid spaces in file paths
- Ensure all environment variables are set correctly

## Build Scripts

### Windows build.bat
```batch
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
```

### Windows build_run.bat
```batch
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
cd bin
.\Lancer-bot.exe
cd ..
echo.
pause
```

## Contributing

This project is maintained by WSU programmers. Contributions are welcome through pull requests.

## License

[Your License Here]
