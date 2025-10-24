## TOYBOX ROADMAP

### v1.1 - A modern toolchain

Split out ToyBox from ChromaGrid, allowing it to be used as a standalone piece of software for building simple mouse driven games.

- [x] Move toybox code into its own git repository
- [x] Separate Makefiles for toybox and ChromaGrid
- [x] Unified Makefile target for Atari target and macOS host
    - [x] Move game loop to `machine_c::with_machine(...)`
    - [x] Add new Xcode project with external build system
- [x] Update to gcc 15.2 mintelf toolchain
    - [x] Use link time optimizations
    - [x] Update to use libcmini top of tree (0.54 too old for elf)
    - [x] Remove libcmini 0.47 workarounds
- [x] Update sources to ~~C++20~~ C++23
- [-] Add unit tests
- [x] Add sample project
- [x] Update documentation
- [x] Add Xcode IDE to toybox project


### v2.0 - A basic general purpose game engine

Support simple horizontally scrolling games controlled with joystick. ETA Summer 2026.

- [ ] `fixed16_t` math library, 12:4 bits
- [x] `screen_c` as a subclass of `canvas_c`
    - [ ] Wrapper for an `image_c` with its own size and offset
    - [ ] Translate & clip drawing primitives to superclass
- [x] Rudimentary `display_list_c` only supporting a single `screen_c` and `palette_c`
    - [x] Use active `display_list_c` not `image_c`
    - [ ] Update Atari target to HW scroll display if needed
    - [ ] Update host to generate _HW scrolled_ display
- [ ] `tilemap_c` for defining a tiled display from 16x16 blocks
    - [ ] Source from `tileset_c`
    - [ ] Support at least two layers of graphics in input
    - [ ] General tile types; empty, solid, climbable, hurts, etc.
- [ ] `entity_c` for defining basic game AI
    - [ ] User controllable entity
    - [ ] Collision with `tilemap_c`
    - [ ] Collision with other entities of set types
- [ ] Implement `audio_mixer_c`
    - [ ] Up to four channels of mixed mono audio
- [ ] Implement `controller_c` to read ST joystick 1 or 0.
    - [ ] Implement basic `state_recognizer_c`
    - [ ] Concrete implementations for taps and holds


### v3.0 - A complete general purpose game engine

Support static one screen or eight way scrolling games with rasters and split-screen. Controlled by mouse, joystick, jagpad and/or keyboard. ETA Summer 2028.

- [ ] `display_list_c` can have several items
    - [ ] Multiple `screen_c` for screen splits
    - [ ] Multiple `palette_c` for palette splits
    - [ ] Multiple `raster_c` for rasters
        - [ ] Define line offset and color index
- [ ] `screen_c` support infinite horizontal and vertical offset
- [ ] More dynamic game entities
    - [ ] Basic _bullet_ AI entity
    - [ ] Basic path following AI entity
    - [ ] AI able to walk on ground
    - [ ] AI able to fly
    - [ ] AI controllable by decisions tree
- [ ] `world_c` and `level_c` as a concept of a persistent game world
    - [ ] Dynamically load a `level_c` from a new `scene_c`
    - [ ] Persist state for a `level_c` when unloaded (picked items, dead enemies, etc.)
    - [ ] Support loading and saving game state
- [ ] Add jagpad support to `controller_c`
- [ ] Add `modmusic_c` as a concrete `music_c` subclass
    

### v4.0 - Support all the things!

- [ ] Amiga target
- [ ] Atari STfm target
- [ ] Jaguar64 target
- [ ] Sega Genesis target

