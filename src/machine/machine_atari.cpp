//
//  machine.cpp
//  toybox
//
//  Created by Fredrik on 2024-04-24.
//

#include "machine/machine.hpp"
#include "machine/timer.hpp"
#include "media/screen.hpp"
#include "media/display_list.hpp"
#include "core/algorithm.hpp"

#if TOYBOX_TARGET_ATARI

using namespace toybox;

extern "C" {
    const palette_c *g_active_palette = nullptr;
    const image_c *g_active_image = nullptr;
}


machine_c *machine_c::_shared_machine = nullptr;

machine_c &machine_c::shared() {
    assert(_shared_machine != nullptr && "Shared machine instance not initialized");
    return *_shared_machine;
}

machine_c::machine_c() {
#ifdef __M68000__
    _old_super = Super(0);
    _old_modes[0] = Blitmode(-1);
    Blitmode(0);
    _old_modes[1] = Getrez();
    Setscreen((void *)-1, (void *)-1, 0);
    _old_modes[2] = *((uint8_t*)0x484);
    *((uint8_t*)0x484) = 0;
    g_active_palette = new palette_c((uint16_t*)0xffff8240);
#else
    g_active_palette = new palette_c();
#endif
    assert(type() != unknown && type() >= ste && "Machine type must be STE or higher");
}

machine_c::~machine_c() {
#ifdef __M68000__
    *((uint8_t*)0x484) = (uint8_t)_old_modes[2];
    Setscreen((void *)-1, (void *)-1, _old_modes[1]);
    Blitmode(_old_modes[0]);
    Super(_old_super);
#endif
}

#ifndef TOYBOX_HOST
int machine_c::with_machine(int argc, const char * argv[], int (*game)(machine_c &machine)) {
    assert(_shared_machine == nullptr && "Shared machine already initialized");
    machine_c machine;
    _shared_machine = &machine;
    return game(machine);
}
#endif

machine_c::type_e machine_c::type() const {
#ifdef __M68000__
    return static_cast<type_e>((get_cookie(0x5F4D4348) >> 16) + 1); // '_MCH'
#else
    return ste;
#endif
}

size_s machine_c::screen_size() const {
    return size_s(320, 200);
}

size_t machine_c::max_memory() const {
#ifdef __M68000__
    return *((uint32_t*)0x436);
#else
    return 0x100000;
#endif
}

size_t machine_c::user_memory() const {
#ifdef __M68000__
    return max_memory() - *((uint32_t*)0x432);
#else
    return max_memory() - 0x10000;
#endif
}


#ifdef __M68000__
struct mem_chunk {
    long valid;
#define VAL_ALLOC 0xa11c0abcL
    struct mem_chunk *next;
    unsigned long size;
};
#define BORDER_EXTRA ((sizeof(struct mem_chunk) + sizeof(long) + 7) & ~7)
void machine_c::free_system_memory() {
    mem_chunk *p = *(mem_chunk **)(0x44e);
    p->valid = VAL_ALLOC;
    p->next = nullptr;
    p->size = 32000;
    p++;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
    _free(p);
#pragma GCC diagnostic pop
}
#else
void machine_c::free_system_memory() {}
#endif

uint32_t machine_c::get_cookie(uint32_t cookie, uint32_t def_value) const {
#ifdef __M68000__
    uint32_t *cookie_jar = *((uint32_t**)0x5A0);
    if (cookie_jar) {
        while ((cookie_jar[0] != 0)) {
            if (cookie_jar[0] == cookie) {
                return cookie_jar[1];
            }
            cookie_jar += 2;
        }
    }
    return def_value;
#else
    return def_value;
#endif
}

static const display_list_c *s_active_display_list = nullptr;

const display_list_c *machine_c::active_display_list() const {
    return s_active_display_list;
}

void machine_c::set_active_display_list(const display_list_c *display_list) {
    s_active_display_list = display_list;
    if (display_list) {
        assert(is_sorted(display_list->begin(), display_list->end()) && "Display list must be sorted");
        for (const auto& entry : *display_list) {
            switch (entry.item.display_type()) {
                case display_item_c::screen:
                    set_active_image(&entry.screen().image());
                    break;
                case display_item_c::palette:
                    set_active_palette(&entry.palette());
                    break;
            }
        }
    } else {
        set_active_image(nullptr);
        set_active_palette(nullptr);
    }
}


void machine_c::set_active_image(const image_c *image, point_s offset) {
    assert(offset.x == 0 && offset.y == 0 && "Offset must be zero");
    timer_c::with_paused_timers([this, image] {
        g_active_image = image;
        if (type() > ste) {
#ifdef __M68000__
            *((uint16_t*)0x452) = 1;
            *((uint16_t **)0x45E) = image->_bitmap.get();
#endif
        }
    });
}

void machine_c::set_active_palette(const palette_c *palette) {
#ifdef __M68000__
#   if TOYBOX_TARGET_ATARI
    copy(palette->begin(), palette->end(), reinterpret_cast<color_c*>(0xffff8240));
#   else
#       error "Unsupported target"
#   endif
    g_active_palette = palette;
#else
    timer_c::with_paused_timers([palette]{
        g_active_palette = palette;
    });
#endif
}

#endif
