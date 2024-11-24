@echo off
echo Building DecoySystem...

:: Create build directory if it doesn't exist
if not exist "build" mkdir build
cd build

:: Run CMake
cmake ..

:: Build the project
cmake --build . --config Release

:: Copy executable to bin directory
if not exist "..\bin" mkdir ..\bin
copy /Y Release\DecoySystem.exe ..\bin\

echo Build complete! Executable is in bin directory.
cd .. 