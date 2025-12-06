//
//  entity.hpp
//  toybox
//
//  Created by Fredrik on 2025-11-11.
//

#pragma once

#include "core/geometry.hpp"
#include "core/memory.hpp"
#include "media/tileset.hpp"
#include "machine/input.hpp"
#include "runtime/tilemap.hpp"

namespace toybox {

    struct entity_s {
        static constexpr const int edata_size = 4;
        static constexpr const int adata_size = 6;
        uint8_t index = 0;
        uint8_t active:1 = 1;  // Only active entities are drawn, run and actions.
        uint8_t event:1 = 0;    // If set action is not called per frame, only on target event trigger
        uint8_t flags:6 = 0;
        uint8_t type = 0;
        uint8_t group = 0;
        uint8_t action = 0;
        uint8_t frame_index = 0;
        frect_s position;
        uint8_t edata[edata_size];
        template<class T> requires (sizeof(T) <= edata_size)
        T& edata_as() { return (T&)(edata[0]); }
        template<class T> requires (sizeof(T) <= edata_size)
        const T& edata_as() const { return (const T&)(edata[0]); }
        uint8_t adata[adata_size];
        template<class T> requires (sizeof(T) <= adata_size)
        T& adata_as() { return (T&)(adata[0]); }
        template<class T> requires (sizeof(T) <= adata_size)
        const T& adata_as() const { return (const T&)(adata[0]); }
    };
    static_assert(sizeof(entity_s) == 24);

    // struct_layout for byte-order swapping
    template<>
    struct struct_layout<entity_s> {
        static constexpr const char* value = "6b4w10b";  // index, type, group, action, frame_index, flags, position(4w), edata[4], adata[6]
    };

    struct entity_type_def_s {
        struct frame_def_s {
            int index;      // -1 do not draw
            rect_s rect;    // Rect of relative to graphics tile
        };
        tileset_c* tileset;
        vector_c<frame_def_s, 0> frame_defs; //
    };
    
}
