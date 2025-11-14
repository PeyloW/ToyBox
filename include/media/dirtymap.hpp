//
//  dirtymap.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-11.
//

#pragma once

#include "core/geometry.hpp"
#include "core/utility.hpp"

namespace toybox {
    
    class image_c;
    class canvas_c;
    namespace detail {
        class basic_canvas_c;
    }

    static_assert(TOYBOX_DIRTYMAP_TILE_SIZE.width % 16 == 0, "Tile width must be a multiple of 16");
    
    /**
     A `dirtymap_c` represents dirty areas of a `canvas_c` that is in need of
     redrawing.
     NOTO: Is support for dirty grids other than 16x16 pixels needed.
     */
    class dirtymap_c : public nocopy_c {
    public:
        using restore_f = function_c<void(const rect_s&)>;
        
        static dirtymap_c* create(size_s size);
        
        void mark(const rect_s &rect);
        void merge(const dirtymap_c &dirtymap);
        void restore(canvas_c &canvas, const image_c &clean_image);
        void restore(restore_f& func);
        void clear();
#if TOYBOX_DEBUG_DIRTYMAP
        void debug(const char *name) const;
#endif
    private:
        static int instance_size(size_s *size);
        dirtymap_c(const size_s size) : _size(size) {}
        const size_s _size;
        uint8_t _data[];
    };
    
}
