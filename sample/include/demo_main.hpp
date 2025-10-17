//
//  demo_main.hpp
//  cgrid
//
//  Created by Fredrik on 2025-10-14.
//

#pragma once

#include "scene.hpp"

using namespace toybox;

class demo_main_scene_c final : public scene_c {
public:
    demo_main_scene_c(scene_manager_c &manager);

    virtual scene_c::configuration_s &configuration() const override;
    virtual void will_appear(screen_c &clear_screen, bool obsured) override;
    virtual void update_back(screen_c &back_screen, int ticks) override;

private:
    mouse_c &_mouse;
    tileset_c &_sprites;
    point_s _pos[64];
};
