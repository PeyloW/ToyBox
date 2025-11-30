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
        static constexpr const uint8_t flag_disabled = 1 << 7;  // Draw no graphics, run no actions, as if it did not exist.
        static constexpr const uint8_t flag_event = 1 << 6;     // If set action is not called per frame, only on target trigger
        static constexpr const int edata_size = 4;
        static constexpr const int adata_size = 14;
        uint8_t index = 0;
        uint8_t type = 0;
        uint8_t group = 0;
        uint8_t action = 0;
        uint8_t frame_index = 0;
        uint8_t flags = 0;
        frect_s position;
        uint8_t edata[4];
        template<class T> requires (sizeof(T) <= edata_size)
        T& edata_as() { return (T&)(edata[0]); }
        uint8_t adata[6];
        template<class T> requires (sizeof(T) <= adata_size)
        T& adata_as() { return (T&)(adata[0]); }
    };
    static_assert(sizeof(entity_s) == 24);

    struct entity_type_def_s {
        struct frame_def_s {
            int index;      // -1 do not draw
            point_s offset;
        };
        tileset_c* tileset;
        vector_c<frame_def_s, 0> frame_defs; //
    };
    
}
