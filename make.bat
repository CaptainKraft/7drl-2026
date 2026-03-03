@echo off
setlocal

set "PROJ_DIR=%~dp0"
if "%PROJ_DIR:~-1%"=="\" set "PROJ_DIR=%PROJ_DIR:~0,-1%"

set "SDIR=%PROJ_DIR%\src"
set "LDIR=%PROJ_DIR%\lib"
set "BDIR=%PROJ_DIR%\build"
set "BIN=game.exe"

set "CC=clang"
set "CFLAGS_COMMON=-std=c11 -I%SDIR% -fdiagnostics-absolute-paths -D_CRT_SECURE_NO_WARNINGS"
set "CFLAGS_DEBUG=-g -O0 -DDEBUG"
set "CFLAGS_RELEASE=-O2 -DNDEBUG"

set "RAYLIB_DIR=%PROJ_DIR%\..\raylib"
set "RAYLIB_BUILD_DIR=%RAYLIB_DIR%\build-clang"
set "RAYLIB_DESKTOP_LIB=%LDIR%\libraylib-win.lib"
set "LFLAGS=%RAYLIB_DESKTOP_LIB% -lopengl32 -lgdi32 -lwinmm -lkernel32 -luser32 -lshell32 -lole32 -lmsvcrt -lucrt -lvcruntime -llegacy_stdio_definitions -loldnames -Wl,/NODEFAULTLIB:libcmt"

set "TARGET=%~1"
if "%TARGET%"=="" set "TARGET=build"

if /I "%TARGET%"=="build" goto :build
if /I "%TARGET%"=="release" goto :release
if /I "%TARGET%"=="init" goto :init
if /I "%TARGET%"=="libs" goto :libs
if /I "%TARGET%"=="run" goto :run
if /I "%TARGET%"=="clean" goto :clean

echo Unknown target: %TARGET%
echo Usage: make.bat [build^|release^|init^|libs^|run^|clean]
exit /b 1

:ensure_init
if not exist "%BDIR%" (
    echo Build directory missing. Run "make.bat init" first.
    exit /b 1
)
if not exist "%RAYLIB_DESKTOP_LIB%" (
    echo Missing %RAYLIB_DESKTOP_LIB%
    echo Run "make.bat libs" first.
    exit /b 1
)
exit /b 0

:build
call :ensure_init || exit /b 1
set "CFLAGS=%CFLAGS_COMMON% %CFLAGS_DEBUG%"
pushd "%BDIR%"
@echo on
%CC% "%SDIR%\*.c" "%BDIR%\*.o" -o "%BDIR%\%BIN%" %CFLAGS% %LFLAGS%
@echo off
popd
exit /b %ERRORLEVEL%

:release
call :ensure_init || exit /b 1
set "CFLAGS=%CFLAGS_COMMON% %CFLAGS_RELEASE%"
pushd "%BDIR%"
@echo on
%CC% "%SDIR%\*.c" "%BDIR%\*.o" -o "%BDIR%\%BIN%" %CFLAGS% %LFLAGS%
@echo off
popd
exit /b %ERRORLEVEL%

:init
if exist "%BDIR%" rd /s /q "%BDIR%"
mkdir "%BDIR%"
mkdir "%BDIR%\assets"
xcopy /E /I /Q /Y "%PROJ_DIR%\assets" "%BDIR%\assets" >nul

where ctags >nul 2>nul
if not errorlevel 1 (
    ctags -R --fields=+iaS --extras=+q --c-kinds=+p "%SDIR%\ray\raylib.h" "%PROJ_DIR%"
)

set "CFLAGS=%CFLAGS_COMMON% %CFLAGS_RELEASE%"
pushd "%BDIR%"
@echo on
%CC% -c "%SDIR%\ck\*.c" "%SDIR%\stb\stb.c" "%SDIR%\microui\microui.c" %CFLAGS%
@echo off
popd
exit /b %ERRORLEVEL%

:libs
if not exist "%LDIR%" mkdir "%LDIR%"
if not exist "%RAYLIB_DIR%" (
    echo Missing raylib checkout at %RAYLIB_DIR%
    echo Clone it with:
    echo   git clone https://github.com/raysan5/raylib "%RAYLIB_DIR%"
    exit /b 1
)

@echo on
cmake -S "%RAYLIB_DIR%" -B "%RAYLIB_BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF
if errorlevel 1 @echo off & exit /b 1
cmake --build "%RAYLIB_BUILD_DIR%" --config Release
if errorlevel 1 @echo off & exit /b 1
copy /Y "%RAYLIB_BUILD_DIR%\raylib\raylib.lib" "%RAYLIB_DESKTOP_LIB%" >nul
@echo off

if not exist "%RAYLIB_DESKTOP_LIB%" (
    echo Failed to copy raylib desktop library.
    exit /b 1
)

echo Copied %RAYLIB_DESKTOP_LIB%
exit /b 0

:run
if not exist "%BDIR%\%BIN%" (
    echo Missing %BDIR%\%BIN%
    echo Run "make.bat build" first.
    exit /b 1
)
pushd "%BDIR%"
@echo on
"%BDIR%\%BIN%"
@echo off
popd
exit /b %ERRORLEVEL%

:clean
if exist "%BDIR%" rd /s /q "%BDIR%"
if exist "%PROJ_DIR%\.build" rd /s /q "%PROJ_DIR%\.build"
exit /b 0
