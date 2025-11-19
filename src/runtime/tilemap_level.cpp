//
//  tilemap_level.cpp
//  toybox_index
//
//  Created by Fredrik on 2025-11-12.
//

#include "core/iffstream.hpp"
#include "media/viewport.hpp"
#include "media/dirtymap.hpp"
#include "runtime/scene.hpp"
#include "runtime/tilemap_level.hpp"

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
 TODO: The file format needs to be revisited once the programatic implemention is complete.
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

tilemap_level_c::tilemap_level_c(rect_s bounds, tileset_c* tileset) : tilemap_c(bounds), _tileset(tileset) {
    // TODO: Is this correct? This requires bounds to be {0,0} for the level tilemap.
    // And we should probably only dirty the visible region is the level is larger than the display size.
    // Size here is depending on the size of the viewport to draw in later. Is max screen size good enough?
    _tiles_dirtymap = dirtymap_c::create(bounds.size);
    set_visible_bounds(bounds);
}

tilemap_level_c::tilemap_level_c(const char* path, tileset_c* tileset) : tilemap_c(rect_s()), _tileset(tileset) {
    // TODO: Just needs to implement all of this.
    iffstream_c file(path);
    iff_group_s form;
    if (!file.good() || !file.first(cc4::FORM, ::cc4::TLMP, form)) {
        hard_assert(0);
        return; // Not a ILBM
    }
}

tilemap_level_c::~tilemap_level_c() {
    assert(0 && "Why?");
}

void tilemap_level_c::update(viewport_c& viewport, int display_id, int ticks) {
    _viewport = &viewport;
    // Update the AI for the level world, and entities
    // NOTE: How to handle AI if dropping frames?
    debug_cpu_color(0x010);
    update_level();
    debug_cpu_color(0x020);
    update_actions();
    // AI may update tiles, so we need to dirty viewports to redraw them
    debug_cpu_color(0x030);
#if TOYBOX_DEBUG_DIRTYMAP
    _tiles_dirtymap->print_debug("tilemap_level_c::update() _tiles_dirtymap");
#endif
    if (_tiles_dirtymap->is_dirty()) {
        auto& manager = scene_manager_c::shared();
        for (int idx = (int)scene_manager_c::front; idx <= (int)scene_manager_c::back; ++idx) {
            auto& viewport = manager.display_list((scene_manager_c::display_list_e)idx).get(display_id).viewport();
            viewport.dirtymap()->merge(*_tiles_dirtymap);
        }
    }
    _tiles_dirtymap->clear();
    // Draw all the tiles, both updates, and previously dirtied by drawing sprites
    debug_cpu_color(0x040);
    draw_tiles();
    // And lastly draw all the sprites needed
    debug_cpu_color(0x050);
    draw_entities();
    _viewport = nullptr;
}

void tilemap_level_c::update_level() {
    // We do nothing in base class, subclasses may use this to update tiles.
    // For animations, or chnaging state completey for times platforms, etc.
}

void tilemap_level_c::update_actions() {
    // NOTE: Will need some optimisation to not run them all eventually.
    for (auto& entity : _all_entities) {
        if (entity.action != 0) {
            _actions[entity.action](*this, entity);
        }
    }
}

void tilemap_level_c::draw_tiles() {
    auto& viewport = active_viewport();
    viewport.with_clipping(false, [&](){
        // Need to capture the dirty map here, so we have one.
        // And then do the restore without dirtymap so we do not dirty it when restoring.
        auto dirtymap = viewport.dirtymap();
        const auto tilemap_height = _tilespace_bounds.size.height;
        assert(dirtymap != nullptr && "Viewport must have dirtymap");
#if TOYBOX_DEBUG_DIRTYMAP
        dirtymap->print_debug("tilemap_level_c::draw_tiles()");
#endif
        dirtymap->mark<dirtymap_c::mark_type_e::mask>(viewport.clip_rect());
#if TOYBOX_DEBUG_DIRTYMAP
        dirtymap->print_debug("tilemap_level_c::draw_tiles() masked");
#endif
        assert(dirtymap->dirty_bounds().contained_by(viewport.clip_rect()));
        viewport.with_dirtymap(nullptr, [&]() {
            auto restore = [&](const rect_s& rect) {
                assert(rect.contained_by(viewport.clip_rect()) && "Viewport must not be dirty outside clip rect");
                const rect_s tile_rect = rect_s(
                    rect.origin.x >> 4, rect.origin.y >> 4,
                    rect.size.width >> 4, rect.size.height >> 4
                );
                point_s at = rect.origin;
                for (int y = tile_rect.origin.y; y <= tile_rect.max_y(); ++y) {
                    at.x = rect.origin.x;
                    if (y >= tilemap_height) {
                        // Tilemap smaller than view port, so clear with black
                        // TODO: Should the tilemap_level_c be forced to have a viewport size as min?
                        viewport.fill(0, rect_s(at, size_s(rect.size.width, 16)));
                    } else {
                        for (int x = tile_rect.origin.x; x <= tile_rect.max_x(); ++x) {
                            const auto& tile = (*this)[x, y];
                            if (tile.index <= 0) {
                                debug_cpu_color(0x043);
                                viewport.fill(-tile.index, rect_s(at, size_s(16, 16)));
                            } else {
                                debug_cpu_color(0x240);
                                viewport.draw_aligned(*_tileset, tile.index, at);
                            }
                            debug_cpu_color(0x040);
                            at.x += 16;
                        }
                    }
                    at.y += 16;
                }
            };
            dirtymap_c::restore_f func(restore);
            dirtymap->restore(func);
        });
    });
}

void tilemap_level_c::draw_entities() {
    auto& viewport = active_viewport();
    // NOTE: This will need to be a list of visible entities eventually
    for (auto& entity : _all_entities) {
        // Draw entity if not explicitly hidden, and have frame definitions.
        if (!(entity.flags & entity_s::flag_hidden)) {
            const auto& ent_def = _entity_type_defs[entity.type];
            if (ent_def.frame_defs.size() > 0) {
                const auto& frame_def = ent_def.frame_defs[entity.frame_index];
                const point_s center = static_cast<point_s>(entity.position.center);
                const point_s at = center + frame_def.offset;
                debug_cpu_color(0x053);
                viewport.draw(*ent_def.tileset, frame_def.index, at);
                debug_cpu_color(0x050);
            }
        }
    }
}

void tilemap_level_c::mark_tiles_dirtymap(point_s point) {
    mark_tiles_dirtymap(rect_s(point, size_s(1,1)));
}
void tilemap_level_c::mark_tiles_dirtymap(rect_s rect) {
    _tiles_dirtymap->mark(rect);
}

bool tilemap_level_c::collides_with_level(fcrect_s& rect) {
    // TODO: Check if center rect collides with tiles
    return false;
}

bool tilemap_level_c::collides_with_entity(fcrect_s& rect, uint8_t in_group, int& indexOut) {
    // TODO: Check if center rect collides with any entity in a group.
    // When checking for collision between entities center rect is treated as a circle of `width` diameter.
    // On collision the index of the entity colided with is returned
    return false;
}

void tilemap_level_c::set_visible_bounds(const rect_s& bounds) {
    // TODO: When changing bounds columns (and eventually rows) of tiles needs to be marked dirty.
    // NOTE: Outside of visible bounds should always be clean.
    _tiles_dirtymap->mark(bounds);
#if TOYBOX_DEBUG_DIRTYMAP
    _tiles_dirtymap->print_debug("tilemap_level_c::set_visible_bounds()");
#endif
    _visible_bounds = bounds;
}

void tilemap_level_c::splice_subtilemap(int index) {
    // TODO: Merge the subtilemap into self, and mark all changes tiles as dirty.
    // NOTE: Stretch goal would be to animate these, but probably not worth the effort.
}
