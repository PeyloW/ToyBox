//
//  input.cpp
//  toybox
//
//  Created by Fredrik on 2024-04-24.
//

#include "machine/input.hpp"
#include "machine/timer.hpp"

using namespace toybox;

static uint8_t g_prev_mouse_buttons;
uint8_t g_mouse_buttons;
static button_state_e g_mouse_button_states[2];
point_s g_mouse_position;

extern "C" {
#ifndef __M68000__
    // Host must call when mouse state changes
    void g_update_mouse(point_s position, bool left, bool right) {
        g_mouse_position = position;
        g_mouse_buttons = (left ? 2 : 0) | (right ? 1 : 0);
    }
#endif
}

mouse_c &mouse_c::shared() {
    static mouse_c s_shared;
    return s_shared;
}

const rect_s &mouse_c::limits() const {
    return _limits;
}

void mouse_c::set_limits(const rect_s &limits) {
    _limits = limits;
    g_mouse_position = point_s(
        limits.origin.x + _limits.size.width / 2,
        limits.origin.y + _limits.size.height / 2
    );
}

static void update_state() {
    int button;
    do_dbra(button, 1) {
        if (g_mouse_buttons & (1 << button)) {
            g_mouse_button_states[button] = button_state_e::pressed;
        } else if (g_prev_mouse_buttons & (1 << button)) {
            g_mouse_button_states[button] = button_state_e::clicked;
        } else {
            g_mouse_button_states[button] = button_state_e::released;
        }
    } while_dbra(button);
    g_prev_mouse_buttons = g_mouse_buttons;
}

bool mouse_c::is_pressed(button_e button) const {
    return (g_mouse_buttons & (1 << (int)button)) != 0;
}

button_state_e mouse_c::state(button_e button) const {
    auto tick = timer_c::shared(timer_c::timer_e::vbl).tick();
    if (tick > _update_tick) {
        update_state();
        _update_tick = tick;
    }
    return g_mouse_button_states[(int)button];
}

point_s mouse_c::position() {
    point_s clamped_point = point_s(
        MIN(_limits.max_x(), MAX(g_mouse_position.x, _limits.origin.x)),
        MIN(_limits.max_y(), MAX(g_mouse_position.y, _limits.origin.y))
    );
    g_mouse_position = clamped_point;
    return clamped_point;
}
