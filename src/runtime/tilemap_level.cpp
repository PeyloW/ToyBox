//
//  tilemap_level.cpp
//  toybox_index
//
//  Created by Fredrik on 2025-11-12.
//

#include "runtime/tilemap_level.hpp"
#include "core/iffstream.hpp"
#include "media/screen.hpp"

using namespace toybox;

namespace cc4 {
    static constexpr cc4_t TLMP("TLMP"); // Tilemap
    static constexpr cc4_t TMHD("TMHD"); // Tilemap Header
    static constexpr cc4_t TILS("TILS"); // The tiles
    static constexpr cc4_t ENTS("ENTS"); // The entities
}


struct tlmp_header_s {
    cc4_t id;      // Map identifier
    rect_s bounds; // Bounds of tilemap.
};
static_assert(sizeof(tlmp_header_s) == 12);

/*
 File format is:
 Tilemap = FORM # { TLMP
    TMHD # { tlmp_header_s }
    TILS # { sizeof(tile_s) * (bounds.size.width * bounds.size.height) bytes }
    ENTS # { ?? bytes TODO: When we do AI } ?
    LIST # { TLMP
        Tilemap +
    } ?
 }
 */

tilemap_level_c::tilemap_level_c(rect_s bounds) : tilemap_c(bounds) {}

tilemap_level_c::tilemap_level_c(const char* path) : tilemap_c(rect_s()) {
    iffstream_c file(path);
    iff_group_s form;
    if (!file.good() || !file.first(cc4::FORM, ::cc4::TLMP, form)) {
        hard_assert(0);
        return; // Not a ILBM
    }
}


void tilemap_level_c::update(screen_c& screen, int ticks) {
    _screen = &screen;
    for (int i = 0; i < ticks; ++i) {
        update_level();
        update_actions();
    }
    draw_tiles();
    draw_entities();
    _screen = nullptr;
}

void tilemap_level_c::update_level() {}

void tilemap_level_c::update_actions() {
    // NOTE: Will need some optimisation to not run them all eventually.
    for (auto& entity : _all_entities) {
        if (entity.action != 0) {
            _actions[entity.action](*this, entity);
        }
    }
}

void tilemap_level_c::draw_tiles() {
    // TODO: Use the dirtymap of the screen to update tiles
    auto& screen = active_screen();
    // This erases sprites, and any updates caused by `update_level` and `update_actions`
}

void tilemap_level_c::draw_entities() {
    // TODO:
}

void tilemap_level_c::dirty_screens(point_s tilespace_point) {}
void tilemap_level_c::dirty_screens(rect_s tilespace_rect) {}

bool tilemap_level_c::collides_with_level(fcrect_s& rect) {
    // TODO:
    return false;
}

bool tilemap_level_c::collides_with_entity(fcrect_s& rect, uint8_t in_group, int& indexOut) {
    // TODO:
    return false;
}

void tilemap_level_c::splice_subtilemap(int index) {
    // TODO:
}
