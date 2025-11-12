# Common variables and rules for both toybox and game Makefiles

LIBCMINI?=../libcmini-0.54
LIBCMINIINC=$(LIBCMINI)/include
LIBCMINILIB=$(LIBCMINI)/build/mshort/mfastcall
LIBCMINIOBJ=$(LIBCMINILIB)/objs
TOYBOX?=../toybox
TOYBOXINC=$(TOYBOX)/include

FLAGS=-DTOYBOX_TARGET_ATARI=2
CFLAGS=-std=c++23 -c -MMD -MP -I $(TOYBOXINC)
LDFLAGS=-L$(TOYBOX)/build -ltoybox

ifeq ($(HOST),sdl2)
	HB_PATH=/opt/homebrew/bin
	INFO=Building for sdl2 host
	CC=clang++
	AR=ar
	FLAGS+=-O0 -g -DTOYBOX_HOST=sdl2
	CFLAGS+=$(shell $(HB_PATH)/sdl2-config --cflags)
	CFLAGS+=-Wno-vla-cxx-extension
	LDFLAGS+=$(shell $(HB_PATH)/sdl2-config --libs)
else
	INFO=Building for atari target
	CC=/opt/cross-mint/bin/m68k-atari-mintelf-c++
	AR=/opt/cross-mint/bin/m68k-atari-mintelf-ar
	FLAGS+=-m68000 -mshort -mfastcall
	FLAGS+=-g0 -DNDEBUG
	FLAGS+=-s
#	FLAGS+=-S
	FLAGS+=-DTOYBOX_DEBUG_CPU=0
	CFLAGS+=-Os -fomit-frame-pointer -fno-threadsafe-statics
	CFLAGS+=-fno-exceptions -Wno-write-strings -Wno-pointer-arith -fno-rtti
	CFLAGS+=-I $(LIBCMINIINC)
	LDFLAGS+=-nostdlib -L$(LIBCMINILIB)/ -lcmini -lgcc
	LDFLAGS+=-Wl,--traditional-format,--stack,16384,--mno-fastalloc
endif
CFLAGS+=$(FLAGS)
LDFLAGS+=$(FLAGS)

# Log
$(info $(INFO))

# Ensure build directory exists
$(shell mkdir -p build)
