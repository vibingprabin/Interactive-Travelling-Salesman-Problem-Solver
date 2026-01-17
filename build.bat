@echo off
echo ========================================
echo Building TSP Solver with Hungarian Algorithm
echo Using FreeGLUT DLL (Dynamic Linking)
echo ========================================

REM Clean previous object files
if exist *.o del *.o

REM Define FREEGLUT_LIB_PRAGMAS to avoid DLL linking issues
set CFLAGS=-DFREEGLUT_LIB_PRAGMAS=0

REM Compile all source files
echo Compiling ComputerGraphics.cpp...
g++ %CFLAGS% -c ComputerGraphics.cpp -o ComputerGraphics.o -I. -Imunkres-cpp/src -Ifreeglut/include
if %errorlevel% neq 0 goto error

echo Compiling City.cpp...
g++ %CFLAGS% -c City.cpp -o City.o -I.
if %errorlevel% neq 0 goto error

echo Compiling TSPAlgorithm.cpp...
g++ %CFLAGS% -c TSPAlgorithm.cpp -o TSPAlgorithm.o -I. -Imunkres-cpp/src
if %errorlevel% neq 0 goto error

echo Compiling MatrixPanel.cpp...
g++ %CFLAGS% -c MatrixPanel.cpp -o MatrixPanel.o -I. -Ifreeglut/include
if %errorlevel% neq 0 goto error

echo Compiling RenderUtils.cpp...
g++ %CFLAGS% -c RenderUtils.cpp -o RenderUtils.o -I. -Ifreeglut/include
if %errorlevel% neq 0 goto error

echo Compiling munkres.cpp...
g++ %CFLAGS% -c munkres-cpp/src/munkres.cpp -o munkres.o -Imunkres-cpp/src
if %errorlevel% neq 0 goto error

echo.
echo Linking with FreeGLUT DLL...
g++ ComputerGraphics.o City.o TSPAlgorithm.o MatrixPanel.o RenderUtils.o munkres.o -o ComputerGraphics.exe -L. -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32
if %errorlevel% neq 0 goto error

echo.
echo ========================================
echo Build successful! 
echo Make sure freeglut.dll is in the same directory as ComputerGraphics.exe
echo Run: ComputerGraphics.exe
echo ========================================
goto end

:error
echo.
echo ========================================
echo Build FAILED!
echo ========================================
exit /b 1

:end
