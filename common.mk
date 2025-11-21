# Common variables and rules for both toybox and game Makefiles

# Function: find-dir-upwards
# Searches parent directories for target directory
define find-dir-upwards
$(strip $(shell \
  dir="$(CURDIR)"; \
  for i in 1 2 3 4 5 6 7 8 9 10; do \
    if [ -d "$$dir/$(1)" ]; then \
      echo "$$dir/$(1)"; \
      break; \
    fi; \
    [ "$$dir" = "/" ] && break; \
    dir=$$(dirname "$$dir"); \
  done \
))
endef

# Auto-discover LIBCMINI by walking up directory tree
LIBCMINI_AUTO := $(call find-dir-upwards,libcmini-0.54)
LIBCMINI ?= $(if $(LIBCMINI_AUTO),$(LIBCMINI_AUTO),../libcmini-0.54)

# Auto-discover TOYBOX by walking up directory tree
TOYBOX_AUTO := $(call find-dir-upwards,toybox)
TOYBOX ?= $(if $(TOYBOX_AUTO),$(TOYBOX_AUTO),../toybox)

LIBCMINIINC=$(LIBCMINI)/include
LIBCMINILIB=$(LIBCMINI)/build/mshort/mfastcall
LIBCMINIOBJ=$(LIBCMINILIB)/objs
TOYBOXINC=$(TOYBOX)/include

FLAGS=-DTOYBOX_TARGET_ATARI=2
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
	FLAGS+=-flto
	STRIP?=on
	INFO=Building for atari target
	CC=/opt/cross-mint/bin/m68k-atari-mintelf-c++
	AR=/opt/cross-mint/bin/m68k-atari-mintelf-gcc-ar
	FLAGS+=-m68000 -mshort -mfastcall
	FLAGS+=-DNDEBUG
ifeq ($(STRIP),on)
	FLAGS+=-s
else
	CFLAGS+=-g3 -ggdb \
		-fvar-tracking-assignments \
		-fno-eliminate-unused-debug-symbols
endif
#	FLAGS+=-S
	FLAGS+=-DTOYBOX_DEBUG_CPU=1
	CFLAGS+=-O3 -fomit-frame-pointer -fno-threadsafe-statics
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
