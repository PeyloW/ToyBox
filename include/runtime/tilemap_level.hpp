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
    
    class tilemap_level_c : public tilemap_c, public asset_c {
    public:
        tilemap_level_c(rect_s tilespace_bounds);
        tilemap_level_c(const char* path);
        
        __forceinline type_e asset_type() const override final { return tilemap_level; }

        virtual void update(screen_c& screen, int ticks);
        virtual void update_level();
        virtual void update_actions();
        virtual void draw_tiles();
        virtual void draw_entities();

        void dirty_screens(point_s tilespace_point);
        void dirty_screens(rect_s tilespace_rect);

        screen_c& active_screen() {
            assert(_screen && "No active canvas");
            return *_screen;
        };

        bool collides_with_level(fcrect_s& rect);
        bool collides_with_entity(fcrect_s& rect, uint8_t in_group, int& indexOut);

        vector_c<action_f, 0>& actions() { return _actions; };
        vector_c<entity_type_def_s, 0>& entity_type_defs() { return _entity_type_defs; };
        vector_c<entity_s, 0>& all_entities() { return _all_entities; }
        
        void splice_subtilemap(int index);
    protected:
        screen_c* _screen;
        rect_s _visible_bounds;
        vector_c<entity_s, 0> _all_entities;
        vector_c<tilemap_c, 0> _subtilemaps;
        vector_c<action_f, 0> _actions;
        vector_c<entity_type_def_s, 0> _entity_type_defs;
    };
    
}
