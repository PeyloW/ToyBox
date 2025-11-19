//
//  viewport.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-17.
//

#pragma once

#include "media/canvas.hpp"
#include "media/display_list.hpp"

namespace toybox {

    /**
     A `viewport_c` is an abstraction for displaying a viewport of content.
     Contains an `image_c` for the bitmap data, and a `dirtymap_c` to restore
     dirty areas.
     */
    static_assert(!is_polymorphic<canvas_c>::value);
    class viewport_c : public display_item_c, public canvas_c {
        friend class machine_c;
    public:
        __forceinline type_e display_type() const override { return viewport; }

        viewport_c(size_s viewport_size = TOYBOX_SCREEN_SIZE_DEFAULT);
        ~viewport_c();

        point_s offset() const { return _offset; }
        void set_offset(point_s offset);

        __forceinline dirtymap_c* dirtymap() const { return _dirtymap; }

    private:
        struct display_config_t {
            uint16_t* bitmap_start;
            uint8_t extra_words;
            uint8_t pixel_shift;
        };
        const display_config_t display_config() const;
        size_s _viewport_size;
        point_s _offset;
        image_c _image;
    };

}
