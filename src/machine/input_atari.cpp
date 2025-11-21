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
    extern void g_mouse_interupt(void*);
    extern timer_c::func_a_t g_system_joystick_interupt;
    extern void g_joystick_interupt(void*);
    static _KBDVECS* g_keyboard_vectors = nullptr;

    static bool g_joystick_reporting = false;
    
    static void init_keyboard_vectors(void) {
        if (g_keyboard_vectors == nullptr) {
            g_keyboard_vectors = Kbdvbase();
        }
    }
#endif
}


mouse_c::mouse_c() : _update_tick(0) {
    set_limits(rect_s(point_s(), machine_c::shared().screen_size()));
#ifdef __M68000__
    if (g_system_mouse_interupt == nullptr) {
        init_keyboard_vectors();
        g_system_mouse_interupt = g_keyboard_vectors->mousevec;
        g_keyboard_vectors->mousevec = &g_mouse_interupt;
    }
    if (g_joystick_reporting) {
        static char s_packer[] = {0x1A, 0x08}; // Relative mouse reporting on
        Ikbdws(1, s_packer);
        g_joystick_reporting = false;
    }
#endif
}

mouse_c::~mouse_c() {
#ifdef __M68000__
    g_keyboard_vectors->mousevec = g_system_mouse_interupt;
#endif
}

controller_c::controller_c(controller_c::port_e port) : _port(port) {
#ifdef __M68000__
    if (g_system_joystick_interupt == nullptr) {
        init_keyboard_vectors();
        g_system_joystick_interupt = g_keyboard_vectors->joyvec;
        g_keyboard_vectors->joyvec = &g_joystick_interupt;
    }
    if (!g_joystick_reporting) {
        static char s_packer[] = {0x12, 0x14}; //Joustick reporting on, mouse off
        Ikbdws(1, s_packer);
        g_joystick_reporting = true;
    }
#endif
}

controller_c::~controller_c() {
#ifdef __M68000__
    if (g_system_joystick_interupt != nullptr) {
        g_keyboard_vectors->joyvec = g_system_joystick_interupt;
        g_system_joystick_interupt = nullptr;
    }
#endif
}

#endif
