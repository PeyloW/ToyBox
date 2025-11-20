//
//  tileset.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-17.
//

#pragma once

#include "media/image.hpp"

namespace toybox {
    
    using namespace toybox;
 
    /**
     A `tileset_c` is a convenience wrapper on top of `image_c` for handling a
     a set of equally sized tiles.
     Tiles can be indexed as column/row using a `point_t`, or by a continuous
     index.
     */
    class tileset_c : public asset_c {
    public:
        tileset_c(const shared_ptr_c<image_c>& image, size_s tile_size);
        virtual ~tileset_c() {};

        __forceinline type_e asset_type() const override final { return tileset; }

        __forceinline const shared_ptr_c<image_c>& image() const __pure {
            return _image;
        }

        __forceinline size_s tile_size() const __pure { return _rects[0].size; }
        
        int16_t max_index() const __pure { return _max_tile.x * _max_tile.y; };
        point_s max_tile() const __pure { return _max_tile; };

        __forceinline const rect_s& operator[](int i) const __pure {
            assert(i >= 0 && i < max_index() && "Tile index out of bounds");
            return _rects[i];
        }
        __forceinline const rect_s& operator[](point_s p) const __pure {
            return (*this)[p.x, p.y];
        }
        __forceinline const rect_s& operator[](int x, int y) const __pure {
            assert(x >= 0 && x < _max_tile.x && y >= 0 && y < _max_tile.y && "Tile coordinates out of bounds");
            return _rects[x + _max_tile.x * y];
        }
    private:
        const shared_ptr_c<image_c> _image;
        const point_s _max_tile;
        unique_ptr_c<rect_s> _rects;
    };

}
