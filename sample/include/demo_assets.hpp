//
//  demo_assets.hpp
//  toybox - sample
//
//  Created by Fredrik on 2025-10-14.
//

#pragma once

#include "runtime/assets.hpp"

using namespace toybox;

enum demo_assets_e {
// Shared
    
// Used by fullscreen_scene
    BACKGROUND, // image
    SPRITES,    // tileset
    MUSIC,      // music
// Used by tilemap_scene
    TILESET_WALL,
    TILESET_SPR,// Tileset for sprites
    LEVEL       // tilemap level
};
