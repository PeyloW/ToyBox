//
//  entity.hpp
//  toybox_index
//
//  Created by Fredrik on 2025-11-11.
//

#pragma once

#include "core/geometry.hpp"
#include "core/memory.hpp"
#include "media/tileset.hpp"

namespace toybox {

    struct entity_s {
        static constexpr const uint16_t flag_hidden = 1 << 0;
        uint8_t index = 0;
        uint8_t type = 0;
        uint8_t group = 0;
        uint8_t action = 0;
        uint8_t frame_index = 0;
        uint8_t flags = 0;
        frect_s position;
        uint8_t edata[4];
        uint8_t adata[14];
        template<class T> requires (sizeof(T) <= 4)
        T& edata_as() { return (T&)(edata[0]); }
        template<class T> requires (sizeof(T) <= 14)
        T& adata_as() { return (T&)(adata[0]); }
    };
    static_assert(sizeof(entity_s) == 32);

    struct entity_type_def_s {
        struct frame_def_s {
            int index;
            point_s offset;
        };
        tileset_c* tileset;
        vector_c<frame_def_s, 0> frame_defs; //
    };
    
}
