# Default to HOST=sdl2 unless explicitly set
include common.mk

# Find all sources recursively
CPP_SOURCES=$(shell find src -name "*.cpp")
CPP_OBJECTS=$(patsubst src/%.cpp,build/%.o,$(CPP_SOURCES))
DEPS=$(CPP_OBJECTS:.o=.d)

OBJECTS=$(CPP_OBJECTS)

ifneq ($(HOST),sdl2)
	ASM_SOURCES=$(shell find src -name "*.S")
	ASM_OBJECTS=$(patsubst src/%.S,build/%.o,$(ASM_SOURCES))
	OBJECTS+=$(ASM_OBJECTS)
endif

# Include dependency files (if they exist)
# Only include .d files that actually exist to avoid Make trying to remake them
ifneq ($(MAKECMDGOALS),clean)
  -include $(wildcard $(DEPS))
endif

# Disable built-in implicit rules
.SUFFIXES:

all: libtoybox.a

libtoybox.a: $(OBJECTS)
	$(AR) rcs build/libtoybox.a $(OBJECTS)

# Preserve directory structure in build/
build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

build/%.o: src/%.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

test: libtoybox.a
	$(MAKE) -C tests clean $(if $(HOST),HOST=$(HOST))
	$(MAKE) -C tests $(if $(HOST),HOST=$(HOST))
	./tests/build/tbtest

clean:
	rm -rf build

.PHONY: all test clean
