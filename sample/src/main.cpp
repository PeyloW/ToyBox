//
//  main.cpp
//  toybox - sample
//
//  Created by Fredrik on 2025-10-12.
//

#include "machine.hpp"
#include "demo_assets.hpp"
#include "demo_main.hpp"
#include "audio_mixer.hpp"

int main(int argc, const char * argv[]) {
    return  machine_c::with_machine(argc, argv, [] (machine_c &m) {
        // Setup and pre-load all assets
        asset_manager_c::set_shared(new demo_asset_manager_c());
        auto &assets = asset_manager_c::shared();
        assets.preload(1);
        
        audio_mixer_c::shared().play(assets.music(MUSIC));
        
        // Setup and start the scene
        scene_manager_c manager(size_s(320, 208));
        auto main_scene = new demo_main_scene_c(manager);
        manager.run(main_scene);

        return 0;
    });
}
