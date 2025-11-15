# TOYBOX

A minimal C++ framework for writing Atari ST<sup>E</sup> entertainment software.

### Project Requirements

* GCC-15.2 with fastcall support (https://tho-otto.de/crossmint.php)
* libcmini (tip of tree) (https://github.com/freemint/libcmini)

## Project Philosophy

ToyBox should be small, fast and convenient. In order to be small, toybox shall use a bare minimum of libcmini, and not include or implement anything not directly needed by a client program. In order to be fast, toybox shall rely on C++ compiler optimizations, and rely on error checking on host machine not M68k target. In order to be convenient, API shall be designed similar to C++ standard library and/or boost.

All code must compile with gcc and clang with C++23 enabled, no standard libraries linked!

Make no assumption of integer/pointer size. Host may use 32 bit integers, target **must** use 16 bit integers. Whenever possible use explicitly sized types, `int16_t` not `short`.

Try to avoid multiple inheritance, and when done only the the first inherited class can be polymorphic. Add statuc asserts before the class definition to ensure this.

Rely on `static_assert` to ensure expected sizes for structs are correct. Asserts are enabled on host, but not on Atari target. Asserts with `hard_assert` are used liberally to ensure correctness.
    
### Game Setup

ToyBox games are intended to run from identical code on an Atari target machine, as on a modern host such as macOS. This is abstracted away in the `machine_c` class.

* `machine_c` the machine singleton.
    * `with_machine` sets up the machine in supervisor mode, or configures the host emulation. Run the game in the provided function or lambda.

### Game Life-cycle

A game is intended to be implemented as a stack of scenes. Navigating to a new screen such as from the menu to a level involves either pushing a new scene onto the stack, or replacing the top scene. Navigating back is popping the stack; popping the last scene exits the game.

* `scene_manager_c` - The manager singleton.
    * `push(...)`, `pop(...)`, `replace(...)` to manage the scene stack.
    * `front`, `back`, `clear` three display lists with viewports: front is being presented, back is being drawn, and clear is used for restoring other viewports from their dirtymaps.
    * `display_list(id)` access a specific display list by ID.
* `scene_c` - The abstract scene baseclass.
    * `configuration()` the scene configuration with viewport_size, buffer_count, and use_clear flag.
    * `will_appear(bool obscured)` called when scene becomes the top scene and will appear.
        * Implement to draw initial content to the clear viewport.
    * `update(display_list_c &display_list, int ticks)` update the scene, drawing to the provided display list's viewport.
* `transition_c` - A transition between two scenes, run for push, pop and replace operations.
* `viewport_c` - A viewport for displaying content, analogous to an Amiga viewport.
    * Subclass of `canvas_c` for drawing operations.
    * Contains an `image_c` for the bitmap buffer and a `dirtymap_c` for dirty region tracking.
* `display_list_c` - A sorted list of display items (viewports and palettes) to be presented.
    * `PRIMARY_VIEWPORT` constant (-1) for the main viewport.
    * `PRIMARY_PALETTE` constant (-2) for the main palette.


### Asset Management

Assets are images, sound effects, music, levels, or any other data the game needs. All assets may not fit in memory at once, and thus need to be loaded and unloaded on demand.

* `asset_manager_c` - The manager singleton, intended to be subclassed for adding typed custom assets.
    * `shared()` get the shared singleton.
    * `image(id)`, `tileset(id)`, `font(id)`, etc. get an asset, load if needed.
    * `preload(sets)` preload assets in batches.
    * `add_asset_def(id, def)` add an asset definition, with an ID, batch sets to be included in, and optionally a lambda for how to load and construct the asset.
* `image_c` an image asset, loaded from EA IFF 85 ILBM files (.iff).
* `sound_c` a sound asset, loaded from Audio Interchange File Format (.aif).
* `ymmusic_c` a music asset, loaded from uncompressed Sound Header files (.snd).
* `font_c` a font asset, based on an image asset, monospace or variable width characters.
* `tileset_c` a tileset asset, based on an image asset, defaults to 16x16 blocks.

### Tilemaps and Levels

ToyBox provides a tilemap system for creating tile-based game worlds with entities and AI.

* `tilemap_c` - A 2D grid of tiles for defining game levels.
    * `tile_s` structure with index, type (none, solid, platform, water), flags, and custom data.
    * Supports accessing tiles with `operator[x, y]`.
    * Can be used as a base class or standalone.
* `tilemap_level_c` - An extended tilemap with entity support and rendering.
    * Inherits from both `tilemap_c` and `asset_c`.
    * Can be loaded from files or created procedurally.
    * `update(viewport_c& viewport, int display_id, int ticks)` to update and render the level.
    * Manages entities with AI actions.
    * Handles dirty region tracking for tile updates.
* `entity_s` - A game entity structure for sprites, enemies, items, etc.
    * Type, group, action index, frame index, flags (e.g., hidden).
    * Position as fixed-point rectangle (`fcrect_s`).
    * Custom data storage for entity-specific and action-specific data.
* `action_f` - Function type for AI actions: `void(*)(tilemap_level_c&, entity_s&)`.
    * Actions are registered in the level's action vector.
    * Each entity references an action by index.
