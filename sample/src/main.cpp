//
//  main.cpp
//  toybox - sample
//
//  Created by Fredrik on 2025-10-12.
//

#include "machine/machine.hpp"
#include "media/audio_mixer.hpp"
#include "runtime/assets.hpp"
#include "demo_assets.hpp"
#include "fullscreen_scene.hpp"

static asset_manager_c& setup_assets() {
    constexpr pair_c<int,asset_manager_c::asset_def_s> asset_defs[] = {
        { BACKGROUND, asset_manager_c::asset_def_s(asset_c::image, 1, "backgrnd.iff") },
        { SPRITES, asset_manager_c::asset_def_s(asset_c::tileset, 1, "sprites.iff", [](const asset_manager_c &manager, const char *path) -> asset_c* {
            shared_ptr_c<image_c> image = new image_c(path, 0);
            constexpr auto table = canvas_c::remap_table_c({
                {1, 10}, {2, 11}, {3, 11}, {4, 12}, {5, 13}, {6, 14}
            });
            canvas_c canvas(*image);
            canvas.remap_colors(table, rect_s(0, 0, 128, 32));
            return new tileset_c(image, size_s(32, 32));
        })},
        { MUSIC, asset_manager_c::asset_def_s(asset_c::music, 1, "music.snd") }
    };

    auto &assets = asset_manager_c::shared();
    for (const auto &asset_def : asset_defs) {
        assets.add_asset_def(asset_def.first, asset_def.second);
    }

    assets.preload(1);

    return assets;
}

int main(int argc, const char * argv[]) {
    return  machine_c::with_machine(argc, argv, [] (machine_c &m) {
        // Setup and pre-load all assets
        auto &assets = setup_assets();
        
        
        audio_mixer_c::shared().play(assets.music(MUSIC));
        
        // Setup and start the scene
        auto main_scene = new fullscreen_scene_c();
        scene_manager_c::shared().run(main_scene);

        return 0;
    });
}
