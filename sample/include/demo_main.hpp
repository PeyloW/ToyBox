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
    demo_main_scene_c();

    virtual scene_c::configuration_s &configuration() const override;
    virtual void will_appear(bool obsured) override;
    virtual void update(display_list_c& display, int ticks) override;

private:
    mouse_c &_mouse;
    tileset_c &_sprites;
    point_s _pos[64];
};
