//
//  tilemap.hpp
//  toybox
//
//  Created by Fredrik on 2025-11-09.
//

#pragma once

#include "media/tileset.hpp"
#include "runtime/entity.hpp"
#include "core/system_helpers.hpp"

namespace toybox {

    struct tile_s {
        enum class type_e : uint8_t {
            none     = 0,
            water    = 1,
            platform = 2,
            solid    = 3,
        };
        using enum type_e;

        int16_t index = 0;  // tileset index to draw with, 0 to -15 
        type_e type = none;    // Tile type
        uint8_t flags = 0;
        uint8_t data[4];
        template<class T>
        requires (sizeof(T) <= 4)
        T& data_as() { return (T&)(&data[0]); }
        
        enum class flag_e : uint16_t {
            climbable = 1 << 0,
        };
        using enum flag_e;
        
    };
    static_assert(sizeof(tile_s) == 8);
    
    
    class tilemap_c : nocopy_c {
    public:
        tilemap_c(const rect_s& tilespace_bounds);
        ~tilemap_c() = default;
        
        __forceinline tile_s& operator[](point_s p) __pure { return (*this)[p.x, p.y]; }
        __forceinline const tile_s& operator[](point_s p) const __pure { return (*this)[p.x, p.y]; }
        __forceinline tile_s& operator[](int x, int y) __pure { return _tiles[x + y * _tilespace_bounds.size.width]; }
        __forceinline const tile_s& operator[](int x, int y) const __pure { return _tiles[x + y * _tilespace_bounds.size.width]; }

    protected:
        rect_s _tilespace_bounds;
        vector_c<tile_s, 0> _tiles;
        vector_c<int,0> _activate_entity_idxs;
    };
    
}
