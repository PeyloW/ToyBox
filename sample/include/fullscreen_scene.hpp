//
//  fullscreen_scene.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-14.
//

#pragma once

#include "runtime/scene.hpp"

using namespace toybox;

class fullscreen_scene_c final : public scene_c {
public:
    fullscreen_scene_c();

    virtual scene_c::configuration_s &configuration() const override;
    virtual void will_appear(bool obscured) override;
    virtual void update(display_list_c& display, int ticks) override;

private:
    mouse_c &_mouse;
    tileset_c &_sprites;
    point_s _pos[64];
};
