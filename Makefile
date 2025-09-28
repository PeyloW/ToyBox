include common.mk

TOYBOX_OBJECTS=build/cpp_runtime.o
TOYBOX_OBJECTS+=build/timer.o build/input.o
TOYBOX_OBJECTS+=build/stream.o build/util_stream.o build/iffstream.o
TOYBOX_OBJECTS+=build/asset.o
TOYBOX_OBJECTS+=build/palette.o build/image.o build/tileset.o build/font.o
TOYBOX_OBJECTS+=build/canvas.o build/canvas_stencil.o build/dirtymap.o
TOYBOX_OBJECTS+=build/audio.o build/audio_atari.o build/audio_mixer_atari.o
TOYBOX_OBJECTS+=build/screen.o build/scene.o build/scene_transition.o
TOYBOX_OBJECTS_ATARI=build/machine_atari.o
TOYBOX_OBJECTS_ATARI+=build/timer_atari.o build/input_atari.o build/system_helpers_atari.o
TOYBOX_OBJECTS_ATARI+=build/canvas_atari.o

all: libtoybox.a

libtoybox.a: $(TOYBOX_OBJECTS) $(TOYBOX_OBJECTS_ATARI)
	$(AR) rcs build/libtoybox.a $(TOYBOX_OBJECTS) $(TOYBOX_OBJECTS_ATARI)

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

build/%.o: src/%.sx
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf build
