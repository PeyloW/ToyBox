//
//  demo_assets.hpp
//  toybox - sample
//
//  Created by Fredrik on 2025-10-14.
//

#pragma once

#include "asset.hpp"

using namespace toybox;

enum demo_assets_e {
    BACKGROUND, // image
    SPRITES,    // tileset
    MUSIC       // music
} __packed;

class demo_asset_manager_c : public asset_manager_c {
public:
    demo_asset_manager_c();
};
