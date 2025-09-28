# Common variables and rules for both toybox and game Makefiles

LIBCMINI=../libcmini
LIBCMINIINC=$(LIBCMINI)/include
LIBCMINILIB=$(LIBCMINI)/mshort/mfastcall
TOYBOX=../toybox
TOYBOXINC=$(TOYBOX)/include
GAME=.
GAMEINC=$(GAME)/include

CC=/opt/cross-mint-OLD/bin/m68k-atari-mint-c++
AR=/opt/cross-mint-OLD/bin/m68k-atari-mint-ar

FLAGS=-m68000 -mshort -mfastcall
FLAGS+=-g0 -DNDEBUG -DTOYBOX_TARGET_ATARI=2
FLAGS+=-s
FLAGS+=-DTOYBOX_DEBUG_CPU=0
CFLAGS=-c -std=c++0x -Os -fomit-frame-pointer -fno-threadsafe-statics
CFLAGS+=-fno-exceptions -Wno-write-strings -Wno-pointer-arith -fno-rtti
CFLAGS+=-I $(LIBCMINIINC) -I $(TOYBOXINC) -I $(GAMEINC) $(FLAGS)
LDFLAGS=-nostdlib -L$(TOYBOX)/build -ltoybox -L$(LIBCMINILIB)/ -lcmini -lgcc $(FLAGS)
LDFLAGS+=-Wl,--traditional-format,--stack,16384,--mno-fastalloc

# Ensure build directory exists
$(shell mkdir -p build)
