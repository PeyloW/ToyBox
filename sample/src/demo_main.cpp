//
//  demo_main.cpp
//  cgrid
//
//  Created by Fredrik on 2025-10-14.
//

#include "demo_main.hpp"
#include "demo_assets.hpp"

demo_main_scene_c::demo_main_scene_c(scene_manager_c &manager) :
    scene_c(manager),
    _mouse(mouse_c::shared()),
    _sprites(asset_manager_c::shared().tileset(SPRITES))
{
    _mouse.set_limits(rect_s(8, 8, 280, 160));
    const auto pos = _mouse.postion();
    for (int i = 0; i < 64; i++) {
        _pos[i] = pos;
    }
};

scene_c::configuration_s &demo_main_scene_c::configuration() const {
    static scene_c::configuration_s config(*asset_manager_c::shared().image(BACKGROUND).palette());
    return config;
}

void demo_main_scene_c::will_appear(screen_c &clear_screen, bool obsured) {
    auto &image = asset_manager_c::shared().image(BACKGROUND);
    clear_screen.draw_aligned(image, point_s(0,0));
    for (int i = 0; i < 16; i++) {
        clear_screen.fill(i, rect_s(i * 20, 198, 20, 2));
    }
}

void demo_main_scene_c::update_back(screen_c &back_screen, int ticks) {
    const auto idx = timer_c::shared(timer_c::timer_e::vbl).tick() % 64;
    const auto pos = _mouse.postion();
    _pos[idx] = pos;
    int i;
    while_dbra_count(i, 4) {
        const int p = (idx - i * 20) % 64;
        back_screen.draw(_sprites, i, _pos[p]);
    }
}
