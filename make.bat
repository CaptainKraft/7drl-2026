@echo off

set bin=game.exe
set warn=-Werror -Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -D_CRT_SECURE_NO_WARNINGS

if "%1" == "release" goto :release
if "%1" == "init"    goto :init
if "%1" == "web"     goto :web
if "%1" == "run"     goto :run

if not exist build (
    echo Call 'make init' first
    exit /b
)

:debug
pushd build
set cflags=-g -std=c11 -fdiagnostics-absolute-paths -I..\src -I..\src\ray
set lflags=-L..\lib -Xlinker -subsystem:console -static
set   libs=..\lib\libraylib-win.a -lopengl32 -lgdi32 -lwinmm -lkernel32 -luser32 -lshell32 -lole32
set    src=..\src\main.c ..\src\game.c .\*.o

@echo on
clang -o output\%bin% %src% %cflags% %warn% %lflags% %libs%
@echo off
popd
goto :done

:release
pushd build
set cflags=-O3 -std=c11 -fdiagnostics-absolute-paths -I..\src -I..\src\ray
set lflags=-L..\lib -Xlinker -subsystem:console -static
set   libs=..\lib\libraylib-win.a -lopengl32 -lgdi32 -lwinmm -lkernel32 -luser32 -lshell32 -lole32
set    src=..\src\main.c ..\src\game.c .\*.o

@echo on
clang -o output\%bin% %src% %cflags% %warn% %lflags% %libs%
@echo off
popd
goto :done

:init
ctags -R --c-kinds=+pf --fields=+iaS --extras=+q ..\src\ray\raylib.h .

rem cleanup old build files and start fresh
rd /s /q build
mkdir build
mkdir build\assets
mkdir build\output
mkdir build\output\assets
copy assets build\output\assets\ >NUL
pushd build

rem compile library source files to object files
set cflags=-g -std=c11 -fdiagnostics-absolute-paths -I..\src -I..\src\ray
set    src=..\src\ck\*.c ..\src\microui\microui.c

@echo on
clang -c %src% %cflags% %warn%
@echo off
popd
goto :done

:web
pushd build
set cflags=-std=c11 -I..\src -I..\src\ray -sINITIAL_MEMORY=1073741824 -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES3
set raylib_lib=..\lib\libraylib-web.a
set preload=--preload-file assets
set   src1=..\src\main.c ..\src\game.c
set   src2=..\src\ck\ck_ui.c ..\src\ck\ck_string.c ..\src\ck\ck_math.c ..\src\ck\ck_memory.c ..\src\microui\microui.c

@echo on
emcc -o game.html %src1% %src2% %cflags% %raylib_lib% -s USE_GLFW=3 -s ASYNCIFY %preload%
@echo off
popd
goto :done

:run
pushd build\output
@echo on
%bin%
@echo off
popd

:done
