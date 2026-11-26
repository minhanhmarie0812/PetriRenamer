@echo off
setlocal
cd /d "%~dp0"
where cmake >nul 2>nul || (echo [LOI] Chua co CMake 3.21+ & pause & exit /b 1)
where ninja >nul 2>nul || (echo [LOI] Chua co Ninja & pause & exit /b 1)
if "%QTDIR%"=="" echo [LUU Y] Nen dat QTDIR, vi du C:\Qt\6.8.3\msvc2022_64
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QTDIR%"
if errorlevel 1 pause & exit /b 1
cmake --build build --config Release
if errorlevel 1 pause & exit /b 1
if not exist dist mkdir dist
copy /y build\PetriRenamerCpp.exe dist\ >nul
where windeployqt >nul 2>nul && windeployqt --release --no-translations dist\PetriRenamerCpp.exe
echo Da tao dist\PetriRenamerCpp.exe
pause
