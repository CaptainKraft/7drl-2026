# Detect OS
OS := $(shell uname -s)

SHELL := /bin/bash

# The Makefile lives in the "Project Directory,"
# so this will get the current directory as soon
# as the Makefile is executed. The := is used to
# evalute the PWD immediately, rather than just =
# which can happen after we've changed directories.
PROJ_DIR := $(PWD)

SDIR  = $(PROJ_DIR)/src
LDIR  = $(PROJ_DIR)/lib
BDIR  = $(PROJ_DIR)/build
WEB_BDIR = $(BDIR)/web
SRC   = $(SDIR)/*.c $(BDIR)/*.o
WEB_SRC = $(SDIR)/*.c $(WEB_BDIR)/*.o
LSRC  = $(SDIR)/ck/*.c $(SDIR)/stb/stb.c $(SDIR)/microui/microui.c
BIN = game

.PHONY: default build release web init run clean

CC = clang
WEB_CC = emcc
CFLAGS_COMMON = -std=c11 -I$(SDIR) -fdiagnostics-absolute-paths
CFLAGS_DEBUG = -g -O0 -DDEBUG
CFLAGS_RELEASE = -O2 -DNDEBUG
CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
WEB_CFLAGS = $(CFLAGS_COMMON) -DPLATFORM_WEB
RAYLIB_DIR = ~/dev/raylib
RAYLIB_DESKTOP_LIB = $(LDIR)/libraylib-mac.a
LFLAGS  = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL $(RAYLIB_DESKTOP_LIB)
RAYLIB_WEB_LIB = $(LDIR)/libraylib-web.a
WEB_LFLAGS = $(RAYLIB_WEB_LIB) --preload-file assets@assets -s USE_GLFW=3 -s INITIAL_MEMORY=67108864 -s ASYNCIFY
WEB_SHELL = $(PROJ_DIR)/tools/web_shell.html

default: build

build:
	pushd $(BDIR); $(CC) $(SRC) -o $(BDIR)/$(BIN) $(CFLAGS) $(LFLAGS)

release: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
release: build

web:
	pushd $(WEB_BDIR); $(WEB_CC) $(WEB_SRC) -o $(BDIR)/index.html $(WEB_CFLAGS) $(WEB_LFLAGS) --shell-file $(WEB_SHELL)
	pushd $(BDIR); zip -r web.zip index.html index.js index.wasm index.data
	#butler push $(BDIR)/web.zip captainkraft/7drl-2026:html5

init: CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
init:
	rm -rf $(BDIR)
	mkdir -p $(BDIR)/assets
	cp -R assets $(BDIR)
	ctags -R --fields=+iaS --extras=+q --c-kinds=+p src/ray/raylib.h .
	pushd $(BDIR); $(CC) -c $(LSRC) $(CFLAGS) $(WARN) -fPIC
	mkdir -p $(WEB_BDIR)/assets
	cp -R assets $(WEB_BDIR)
	pushd $(WEB_BDIR); $(WEB_CC) -c $(LSRC) $(WEB_CFLAGS) $(WARN) -fPIC

libs:
	mkdir -p $(PROJ_DIR)/lib
	pushd $(RAYLIB_DIR)/src; make PLATFORM=PLATFORM_DESKTOP clean
	pushd $(RAYLIB_DIR)/src; make PLATFORM=PLATFORM_DESKTOP
	cp $(RAYLIB_DIR)/src/libraylib.a $(RAYLIB_DESKTOP_LIB)
	pushd $(RAYLIB_DIR)/src; make PLATFORM=PLATFORM_WEB clean
	pushd $(RAYLIB_DIR)/src; make PLATFORM=PLATFORM_WEB
	cp $(RAYLIB_DIR)/src/libraylib.web.a $(RAYLIB_WEB_LIB)

run:
	pushd $(BDIR); ./$(BIN)

clean:
	rm -rf ./build; rm -rf ./.build
