//
//  fullscreen_scene.cpp
//  cgrid
//
//  Created by Fredrik on 2025-10-14.
//

#include "fullscreen_scene.hpp"
#include "demo_assets.hpp"

fullscreen_scene_c::fullscreen_scene_c() :
    scene_c(),
    _mouse(mouse_c::shared()),
    _sprites(asset_manager_c::shared().tileset(SPRITES))
{
    _mouse.set_limits(rect_s(8, 8, 280, 160));
    const auto pos = _mouse.position();
    for (int i = 0; i < 64; i++) {
        _pos[i] = pos;
    }
};

scene_c::configuration_s &fullscreen_scene_c::configuration() const {
    static scene_c::configuration_s config{size_s(320, 208), asset_manager_c::shared().image(BACKGROUND).palette(), 2, true};
    return config;
}

void fullscreen_scene_c::will_appear(bool obsured) {
    auto &clear_display = manager.display_list(scene_manager_c::display_list_e::clear);
    auto &clear_viewport = clear_display.get(PRIMARY_VIEWPORT).viewport();
    auto &image = asset_manager_c::shared().image(BACKGROUND);
    clear_viewport.draw_aligned(image, point_s(0,0));
    for (int i = 0; i < 16; i++) {
        clear_viewport.fill(i, rect_s(i * 20, 198, 20, 2));
    }
}

void fullscreen_scene_c::update(display_list_c& display_list, int ticks) {
    auto &back_viewport = display_list.get(PRIMARY_VIEWPORT).viewport();
    const auto idx = timer_c::shared(timer_c::timer_e::vbl).tick() % 64;
    const auto pos = _mouse.position();
    _pos[idx] = pos;
    int i;
    while_dbra_count(i, 4) {
        const int p = (idx - i * 20) % 64;
        back_viewport.draw(_sprites, i, _pos[p]);
    }
}
