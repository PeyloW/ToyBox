# TOYBOX

A minimal C++ framework for writing Atari ST<sup>E</sup> entertainment software.

### Project Requirements

* GCC-15.2 with fastcall support (https://tho-otto.de/crossmint.php)
* libcmini tot (https://github.com/freemint/libcmini)

## Project Philosophy

Toybox should be small, fast and convenient. In order to be small, toybox shall use a bare minimum of libcmini, and not include or implement anything not directly needed by a client program. In order to be fast, toybox shall rely on C++ compiler optimizations, and rely on error checking on host machine not M68k target. In order to be convenient, API shall be designed similar to C++ standard library and/or boost.

All code must compile with gcc and clang with C++23 enabled, no standard libraries linked!

Make no assumption of integer/pointer size. Host may use 32 bit integers, target **must** use 16 bit integers. Whenever possible use explicitly sized types, `int16_t` not `short`.

Rely on `static_assert` to ensure expected sizes for structs are correct. Asserts are enabled on host, but not on Atari target. Asserts with `hard_assert` are used liberally to ensure correctness.
    
### Game Setup

ToyBox games are intended to run from identical code on an Atari target machine, as on a modern host such as macOS. This is abstracted away in the `machine_c` class.

* `machine_c` the machine singleton.
    * `with_machine` sets up the machine in supervisor mode, or configures the host emulation. Run the game in the provided function or lambda.

### Game Life-cycle

A game is intended to be implemented as a stack of scenes. Navigating to a new screen such as from the menu to a level involves either pushing a new scene onto the stack, or replacing the top scene. Navigating back is popping the stack; popping the last scene exits the game.

* `scene_manager_c` - The manager singleton.
    * `push(...)`, `pop(...)`, `replace(...)` to manage the scene stack.
    * `overlay_scene` a scene to draw on top of all other content, such as a status bar or mouse cursor.
    * `front`, `back`, `clear` three screens: front is being presented, back is being drawn, and clear is used for restoring other screens from their dirtymaps.
* `scene_c` - The abstract scene baseclass.
    * `configuration` the scene configuration, only the palette to use for now.
    * `will_appear` called when scene becomes the top scene and will appear.
        * Implement to draw initial content.
    * `update_clear` update the clear screen.
    * `update_back` update the back screen that will be presented next screen swap.
* `transition_c` - A transitions between two scenes, run for push, pop and replace operations.


### Asset Management

Assets are images, sound effects, music, levels, or any other data the game needs. All assets may not fit in memory at once, and thus need to be loaded and unloaded on demand.

* `asset_manager_c` - The manager singleton, intended to be subclassed for adding typed custom assets.
    * `set_shared` set a custom shared singleton.
    * `shared` get the shared singleton, create one if not set.
    * `image`, `tileset`, `font`, etc. get an asset, load if needed.
    * `preload` preload assets in batches.
    * `add_asset_def` add an asset definition, with an ID, batch sets to be included in, and optionally a lambda for how to load and construct the asset.
* `image_c` an image asset, loaded from EA IFF 85 ILBM files (.iff).
* `sound_c` a sound asset, loaded from Audio Interchange File Format (.aif).
* `ymmusic_c` a music asset, loaded from uncompressed Sound Header files (.snd).
* `font_c` a font asset, based on an image asset, monospace or variable width characters.
* `tileset_c` a tileset asset, based on an image asset, defaults to 16x16 blocks.
