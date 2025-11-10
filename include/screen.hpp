//
//  screen.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-17.
//

#pragma once

#include "canvas.hpp"
#include "display_list.hpp"

namespace toybox {
    
    /**
     A `screen_c` is an abstraction for displaying a screen of content.
     Contains an `image_c` for the bitmap data, and a `dirtymap_c` to restore
     dirty areas.
     */
    class screen_c : public canvas_c, public display_item_c {
    public:
        __forceinline type_e display_type() const override { return screen; }

        screen_c(size_s screen_size = TOYBOX_SCREEN_SIZE_DEFAULT);
        ~screen_c();

        __forceinline dirtymap_c* dirtymap() const { return _dirtymap; }

    private:
        image_c _image;
    };
    
}
