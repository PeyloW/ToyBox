//
//  demo_assets.cpp
//  toybox - sample
//
//  Created by Fredrik on 2025-10-14.
//

#include "demo_assets.hpp"
#include "tileset.hpp"
#include "canvas.hpp"

demo_asset_manager_c::demo_asset_manager_c() {
    constexpr pair_c<int,asset_def_s> asset_defs[] = {
        { BACKGROUND, asset_def_s(asset_c::type_e::image, 1, "backgrnd.iff") },
        { SPRITES, asset_def_s(asset_c::type_e::tileset, 1, "sprites.iff", [](const asset_manager_c &manager, const char *path) -> asset_c* {
            shared_ptr_c<image_c> image = new image_c(path, 0);
            constexpr auto table = canvas_c::remap_table_c({
                {1, 10}, {2, 11}, {3, 11}, {4, 12}, {5, 13}, {6, 14}
            });
            canvas_c canvas(*image);
            canvas.remap_colors(table, rect_s(0, 0, 128, 32));
            return new tileset_c(image, size_s(32, 32));
        })},
        { MUSIC, asset_def_s(asset_c::type_e::music, 1, "music.snd") }
    };

    for (const auto &asset_def : asset_defs) {
        add_asset_def(asset_def.first, asset_def.second);
    }

}
