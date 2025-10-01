include common.mk

# Source files for toybox
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(patsubst src/%.cpp,build/%.o,$(SOURCES))
ifneq ($(HOST),sdl2)
	ASM_SOURCES=$(wildcard src/*.S)
	SOURCES+=$(ASM_SOURCES)
	OBJECTS+=$(patsubst src/%.S,build/%.o,$(ASM_SOURCES))
endif

all: libtoybox.a

libtoybox.a: $(OBJECTS)
	$(AR) rcs build/libtoybox.a $(OBJECTS)

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

build/%.o: src/%.S
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf build

.PHONY: all clean
