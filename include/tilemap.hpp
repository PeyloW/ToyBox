//
//  tilemap.hpp
//  toybox
//
//  Created by Fredrik on 2025-11-09.
//

#pragma once

#include "tileset.hpp"
#include "entity.hpp"

namespace toybox {

    struct __packed_struct tile_s {
        uint8_t index;
        uint8_t attr;
        uint16_t userdata;

        enum base_e : uint8_t {
            none     = 0,
            solid    = 1,
            platform = 2,
            water    = 3,
            BASE_MASK     = 0x03
        };
        using enum base_e;

        enum flag_e : uint8_t {
            climbable = 1 << 2,
            FLAG_MASK  = 0b11111100, // upper 6 bits reserved for flags
        };
        using enum flag_e;

        base_e base_type() const { return static_cast<base_e>(attr & BASE_MASK); }
        bool is_climbable() const { return attr &climbable; }
    };
    static_assert(sizeof(tile_s) == 4);
    
    
    class tilemap_c : public asset_c {
    public:
        
        tilemap_c(const rect_s& bounds);
        tilemap_c(const char* path);
        virtual ~tilemap_c() = default;

        __forceinline type_e asset_type() const override final { return tilemap; }
        
        __forceinline tile_s& operator[](point_s p) __pure { return (*this)[p.x, p.y]; }
        __forceinline const tile_s& operator[](point_s p) const __pure { return (*this)[p.x, p.y]; }
        __forceinline tile_s& operator[](int x, int y) __pure { return _tiles[x + y * _bounds.size.width]; }
        __forceinline const tile_s& operator[](int x, int y) const __pure { return _tiles[x + y * _bounds.size.width]; }

    private:
        rect_s _bounds;
        vector_c<tile_s, 0> _tiles;
        list_c<entity_c, 256> _entities; // TODO: When we get to AI
        vector_c<tilemap_c, 0> _sub_tilemaps;
    };
    
}
