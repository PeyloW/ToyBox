//
//  input.cpp
//  toybox
//
//  Created by Fredrik on 2024-04-24.
//

#include "machine/input.hpp"
#include "machine/machine.hpp"
#include "machine/timer.hpp"

#if TOYBOX_TARGET_ATARI

using namespace toybox;

extern "C" {
#ifdef __M68000__
    extern timer_c::func_a_t g_system_mouse_interupt;
    extern void g_mouse_interupt(void *);
    static _KBDVECS *g_keyboard_vectors = nullptr;
#endif
}

mouse_c::mouse_c() : _update_tick(0) {
    set_limits(rect_s(point_s(), machine_c::shared().screen_size()));
#ifdef __M68000__
    g_keyboard_vectors = Kbdvbase();
    g_system_mouse_interupt = g_keyboard_vectors->mousevec;
    g_keyboard_vectors->mousevec = &g_mouse_interupt;
#endif
}

mouse_c::~mouse_c() {
#ifdef __M68000__
    g_keyboard_vectors->mousevec = g_system_mouse_interupt;
#endif
}

#endif
