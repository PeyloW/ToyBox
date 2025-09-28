# TOYBOX

A minimal C++ framework for writing Atari ST entertainment software.

### Project Requirement

* GCC-4.6.4 with fastcall support (https://github.com/PeyloW/gcc-4.6.4)
* libcmini-0.47 (https://github.com/freemint/libcmini)

## Project philosofy

Toybox should be small, fast and convenient. In order to be small toybox shall use a bare minimum of libcmini, and not inlcude or implement anything not directly needed by a client program. In order to be fast toybox shall rely on C++ compiler optimizations, and rely on error checking on host machine not M68k target. In order to be convenient API shall be designed similar to C++ standard library and/or boost.

Types uses suffixes:

* `_t` - Plain typedef of POD
* `_e` - Enumeration
* `_s` - Struct
* `_c` - Class

Structs should be primary about member variable, and all members are public.

Classes should be primary about member functions, and member variables pribate or protected. To avoid accidental expensive copy operations classes should inherit publicly from `nocopy_c` in order to delete copy construction and assignment.

All member functions and non-POD arguments should be const when possible. Structs and classes with a `sizeof` grater than 4 should be bassed as references. Pass objects as pointer only for out arguments, or to signify that an object is optional.

### Game life-cycle

A game is intended to be implemnted as a stack of scenes. Navigating to a new screen such as from the menu to a level involves either pushing a new scene onto the stack, or replacing the top scene. Navigating back is popping the stack, popping the last scenes exits the game.

* `scene_manager_c` - The manager singleton.
    * `push(...`, `pop(...`, `replace(...` to manage the scene stack.
    * `overlay_scene` a scene to draw ontop of all other content, such as a status bar or mouse cursor.
    * `front`, `back`, `clear` three screens, front is being presented, back is being drawn, and clear is used for restoring other screens from their dirtymaps.
* `scene_c` - The abstract scene baseclass.
    * `configuration` the scene configuration, only the palette to use for now.
    * `will_appear` called when scene becomes the top scene and will appear.
        * Implement to draw initial content.
    * `update_clear` updatye the clear screen.
    * `update_back` update the back screen that will be presented next screen swap.
* `transition_c` - A transitions between two scenes, run for push, pop and replace operations.
    
    
## TODO: Future direction

### c1.1 - Modernized toolchain
- [x] Move toybox code into its own git repository
- [x] Seperate Makefiles for toybox and ChromaGrid
- [ ] Unified Makefile target for Atari target and macOS host
    - [ ] Move game loop to `machine_c::with_machine(...)`
    - [ ] Move macOS Xcode setup to external build system
- [ ] Update to gcc 15.2 mintelf toolchain
    - [ ] Use link time optimizations
    - [ ] Update sources to C++20
- [ ] Update to use libcmini v0.54


### v2.0 - Support horizontally scrolling game played with joystick
- [ ] `fixed16_t` math library, 12:4 bits
- [ ] `screen_c` as a subclass of `canvas_c`
    - [ ] Wrapper for an `image_c` with its own size and offset
    - [ ] Translate & clip drawing primitives to superclass
- [ ] Rudimentary `display_list_c` only supporting a single `screen_c`
    - [ ] Use active `display_list_c` not `image_c`
    - [ ] Update Atari target to HW scroll display if needed
    - [ ] Update host to gneerate _HW scrolled_ display
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
    - [ ] Implement `state_recognizer_c`
    - [ ] Concrete implementations for taps and holds


### v2.5 - Support eight way scrolling _metroidvania_-like game
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
    - [ ] AI controllable by decissions tree
- [ ] `world_c` and `level_c` as a concept of a persistent game world
    - [ ] Dynamically load a `level_c` from a new `scene_c`
    - [ ] Persist state for a `level_c` when unloaded (picked items, dead enemies, etc)
    - [ ] Support loading and saving game state
- [ ] Add jagpad support to `controller_c`
- [ ] Add `modmusic_c` as a concrete `music_c` subclass
    

### v3.0 - Support all the things!
- [ ] Amiga target
- [ ] Atari STfm target
- [ ] Jaguar64 target
- [ ] Linux host
- [ ] Window host

