# Common variables and rules for both toybox and game Makefiles

LIBCMINI?=../libcmini-0.54
LIBCMINIINC=$(LIBCMINI)/include
LIBCMINILIB=$(LIBCMINI)/build/mshort/mfastcall
LIBCMINIOBJ=$(LIBCMINILIB)/objs
TOYBOX?=../toybox
TOYBOXINC=$(TOYBOX)/include

FLAGS=-DTOYBOX_TARGET_ATARI=2 -flto
CFLAGS=-std=c++23 -c -MMD -MP -I $(TOYBOXINC)
LDFLAGS=-L$(TOYBOX)/build -ltoybox

HOST?=sdl2
ifeq ($(HOST),sdl2)
	STRIP?=off
	INFO=Building for sdl2 host
	HB_PATH=/opt/homebrew/bin
	CC=clang++
	AR=ar
	FLAGS+=-Og -g -DTOYBOX_HOST=sdl2
ifeq ($(STRIP),on)
	FLAGS+=-s
endif
	CFLAGS+=$(shell $(HB_PATH)/sdl2-config --cflags)
	CFLAGS+=-Wno-vla-cxx-extension -Werror
	LDFLAGS+=$(shell $(HB_PATH)/sdl2-config --libs)
else ifeq ($(HOST),none)
	STRIP?=on
	INFO=Building for atari target
	CC=/opt/cross-mint/bin/m68k-atari-mintelf-c++
	AR=/opt/cross-mint/bin/m68k-atari-mintelf-gcc-ar
	FLAGS+=-m68000 -mshort -mfastcall
	FLAGS+=-DNDEBUG
ifeq ($(STRIP),on)
	FLAGS+=-s
endif
#	FLAGS+=-S
	FLAGS+=-DTOYBOX_DEBUG_CPU=0
	CFLAGS+=-Os -fomit-frame-pointer -fno-threadsafe-statics
	CFLAGS+=-fgcse-after-reload -fpredictive-commoning -ftree-partial-pre -funswitch-loops
	CFLAGS+=-fno-exceptions -Wno-write-strings -Wno-pointer-arith -Wno-packed-not-aligned -fno-rtti
	CFLAGS+=-I $(LIBCMINIINC)
	LDFLAGS+=-nostdlib -L$(LIBCMINILIB)/ -lcmini -lgcc
	LDFLAGS+=-Wl,--traditional-format,--stack,16384,--mno-fastalloc
else
$(error HOST must be 'sdl2' or 'none'. Got: '$(HOST)')
endif
CFLAGS+=$(FLAGS)
LDFLAGS+=$(FLAGS)

# Log
$(info $(INFO))

# Ensure build directory exists
$(shell mkdir -p build)
