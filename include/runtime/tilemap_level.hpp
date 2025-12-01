//
//  level.hpp
//  toybox_index
//
//  Created by Fredrik on 2025-11-12.
//

#pragma once

#include "runtime/actions.hpp"
#include "runtime/tilemap.hpp"

namespace toybox {

    class dirtymap_c;
    
    static_assert(!is_polymorphic<tilemap_c>::value);
    class tilemap_level_c : public asset_c, public tilemap_c {
    public:
        tilemap_level_c(rect_s tilespace_bounds, tileset_c* tileset);
        tilemap_level_c(const char* path, tileset_c* tileset);
        ~tilemap_level_c();
        
        __forceinline type_e asset_type() const override final { return tilemap_level; }

        const char* name() const { return _name.get(); }
        
        virtual void update(viewport_c& viewport, int display_id, int ticks);
        virtual void update_level();
        virtual void update_actions();
        virtual void draw_tiles();
        virtual void draw_entities();

        void mark_tiles_dirtymap(point_s point);
        void mark_tiles_dirtymap(rect_s rect);

        viewport_c& active_viewport() {
            assert(_viewport && "No active viewport");
            return *_viewport;
        };

        tile_s::type_e collides_with_level(int index) const;
        tile_s::type_e collides_with_level(fpoint_s at) const;
        tile_s::type_e collides_with_level(const frect_s& rect) const;
        bool collides_with_entity(int index, uint8_t in_group, int* index_out) const;
        bool collides_with_entity(const frect_s& rect, uint8_t in_group, int* index_out) const;

        vector_c<action_f, 0>& actions() { return _actions; };
        const vector_c<action_f, 0>& actions() const { return _actions; };
        
        vector_c<entity_type_def_s, 0>& entity_type_defs() { return _entity_type_defs; };
        const vector_c<entity_type_def_s, 0>& entity_type_defs() const { return _entity_type_defs; };

        void update_entity_indexes(int from = 0);
        vector_c<entity_s, 0>& all_entities() { return _all_entities; }
        const vector_c<entity_s, 0>& all_entities() const { return _all_entities; }
        entity_s& spawn_entity(uint8_t type, uint8_t group, frect_s position);
        void destroy_entity(int index);
        void erase_destroyed_entities();

        const rect_s&visible_bounds() const { return _visible_bounds; };
        void set_visible_bounds(const rect_s& bounds);
        
        void splice_subtilemap(int index);
    protected:
        viewport_c* _viewport;
        dirtymap_c* _tiles_dirtymap;
        rect_s _visible_bounds;
        tileset_c* _tileset;
        unique_ptr_c<const char> _name;
        vector_c<entity_s, 0> _all_entities;
        vector_c<tilemap_c, 0> _subtilemaps;
        vector_c<action_f, 0> _actions;
        vector_c<entity_type_def_s, 0> _entity_type_defs;
        vector_c<int, 16> _destroy_entities;
    };
    
    
    // Shared file format structures for level editor and game runtime
    namespace detail {
        // EA IFF 85 chunk IDs
        namespace cc4 {
            static constexpr toybox::cc4_t LEVL("LEVL");
            static constexpr toybox::cc4_t LVHD("LVHD");
            static constexpr toybox::cc4_t ENTS("ENTS");
        }
        // Level header for EA IFF 85 LVHD chunk
        struct level_header_s {
            size_s size;                // Size of level in tiles
            uint8_t tileset_index;      // Index of tileset to use
            uint8_t entity_count;       // Number of entities in level
        };
        static_assert(sizeof(level_header_s) == 6);
    }
    // struct_layout for byte-order swapping
    template<>
    struct struct_layout<detail::level_header_s> {
        static constexpr const char* value = "2w2b";  // size.width, size.height, tileset_index, entity_count
    };

}
