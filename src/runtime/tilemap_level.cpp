//
//  tilemap_level.cpp
//  toybox_index
//
//  Created by Fredrik on 2025-11-12.
//

#include "core/iffstream.hpp"
#include "media/screen.hpp"
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
    _tiles_dirtymap = dirtymap_c::create(bounds.size);
    _tiles_dirtymap->mark(bounds);
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


void tilemap_level_c::update(screen_c& screen, int display_id, int ticks) {
    _screen = &screen;
    _tiles_dirtymap->clear();
    // Update the AI for the level world, and entities
    // NOTE: How to handle AI if dropping frames?
    update_level();
    update_actions();
    // AI may update tiles, so we need to dirty screens to redraw them
    auto& manager = scene_manager_c::shared();
    for (int idx = (int)scene_manager_c::front; idx <= (int)scene_manager_c::back; ++idx) {
        auto& screen = manager.display_list((scene_manager_c::display_list_e)idx).get(display_id).screen();
        screen.dirtymap()->merge(*_tiles_dirtymap);
    }
    // Draw all the tiles, both updates, and previously dirtied by drawing sprites
    draw_tiles();
    // And lastly draw all the sprites needed
    draw_entities();
    _screen = nullptr;
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
    auto& screen = active_screen();
    screen.with_clipping(false, [&](){
        screen.with_dirtymap(nullptr, [&]() {
            auto restore = [&](const rect_s& rect) {
                const rect_s tile_rect = rect_s(
                    rect.origin.x >> 4, rect.origin.y >> 4,
                    rect.size.width >> 4, rect.size.height >> 4
                );
                point_s at = rect.origin;
                for (int y = tile_rect.origin.y; y <= tile_rect.max_y(); ++y) {
                    at.x = rect.origin.x;
                    for (int x = tile_rect.origin.x; x <= tile_rect.max_x(); ++x) {
                        const auto& tile = (*this)[x, y];
                        if (tile.index <= 0) {
                            screen.fill(-tile.index, rect_s(at, size_s(16, 16)));
                        } else {
                            screen.draw(*_tileset, tile.index, at);
                        }
                        at.x += 16;
                    }
                    at.y += 16;
                }
            };
            dirtymap_c::restore_f func(restore);
            screen.dirtymap()->restore(func);
        });
    });
}

void tilemap_level_c::draw_entities() {
    auto& screen = active_screen();
    // NOTE: This will need to be a list of visible entities eventually
    for (auto& entity : _all_entities) {
        // Draw entity if not explicitly hidden, and have frame definitions.
        if (!(entity.flags & entity_s::flag_hidden)) {
            const auto& ent_def = _entity_type_defs[entity.type];
            if (ent_def.frame_defs.size() > 0) {
                const auto& frame_def = ent_def.frame_defs[entity.frame_index];
                const point_s center = static_cast<point_s>(entity.position.center);
                const point_s at = center - frame_def.offset;
                screen.draw(*ent_def.tileset, frame_def.index, at);
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

void tilemap_level_c::splice_subtilemap(int index) {
    // TODO: Merge the subtilemap into self, and mark all changes tiles as dirty.
    // NOTE: Stretch goal would be to animate these, but probably not worth the effort.
}
