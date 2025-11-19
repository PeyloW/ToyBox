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
    
    /**
     A `dirtymap_c` represents dirty areas of a `canvas_c` that is in need of
     redrawing.
     NOTO: Is support for dirty grids other than 16x16 pixels needed.
     */
    class dirtymap_c : public nocopy_c {
    public:
        using restore_f = function_c<void(const rect_s&)>;
        static constexpr size_s tile_size = size_s(16, 16);
        
        static dirtymap_c* create(size_s size);
        
        __forceinline size_s size() const { return size_s(_byte_layout_size.width * 16, _byte_layout_size.height * 16); }

        enum class mark_type_e : uint8_t { dirty, clean, mask };
        template<mark_type_e = mark_type_e::dirty>
        void mark(const rect_s &rect);
        void merge(const dirtymap_c &dirtymap);
        bool is_dirty() const { return _is_dirty; }
        void restore(canvas_c &canvas, const image_c &clean_image);
        void restore(restore_f& func);
        void clear();
        
        rect_s dirty_bounds() const;  // Intended for host debugging
        void print_debug(const char *name) const; // Intended for host debugging
    private:
        static int instance_size(size_s size);
        dirtymap_c(const size_s size);
        const int16_t _tilespace_width;
        const size_s _byte_layout_size;
        bool _is_dirty;
        uint8_t __padding;
        uint8_t _data[];    // _data **must** be on an even address.
    };

}
