# TOYBOX

A minimal C++ framework for writing Atari ST entertainment software.

### Project Requirement

* GCC-15.2 with fastcall support (https://tho-otto.de/crossmint.php)
* libcmini tot (https://github.com/freemint/libcmini)

## Project philosofy

Toybox should be small, fast and convenient. In order to be small toybox shall use a bare minimum of libcmini, and not inlcude or implement anything not directly needed by a client program. In order to be fast toybox shall rely on C++ compiler optimizations, and rely on error checking on host machine not M68k target. In order to be convenient API shall be designed similar to C++ standard library and/or boost.

All code must compile with gcc and clang with C++23 enabled, no standard libraries linked!

Make no assumption of integer/pointer size. Host may use 32 bit integers, target **must** use 16 bit integers. Whenever possible use explicitly sized types, `int16_t` not `short`.

Rely on `static_assert` to ensure expected sizes for structs are correct. Asserts are enabled on host, but not Atari target. Asserts with `hard_assert` are used liberly to ensure correctness.
    
Types uses suffix, variables does not:

* `_t` - POD, plain old type.
* `_e` - Enumeration.
* `_s` - Simple structs
    * Must never implement constructors or destructors.
    * For direct access to all members.
* `_c` - Classes
    * Classes **must** support move semantics.
    * **Never** assume copy semantics are available.
        * Most classes explicitly forbig copy semntions by use of `nocopy_c` subclassing.

Variables use optional prefixes:

* `_` - Private/protected member variable.
* `s_` - Static variable.
* `g_` - Global variable.

Variables with no prefix is a local variable, public member, or function argument.



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

### v1.1 - Modernized toolchain
- [x] Move toybox code into its own git repository
- [x] Seperate Makefiles for toybox and ChromaGrid
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
- [ ] Update documentation 
- [ ] Add Xcode IDE to toybox project


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

