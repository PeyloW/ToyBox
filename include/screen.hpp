//
//  screen.hpp
//  toybox
//
//  Created by Fredrik on 2024-04-17.
//

#ifndef screen_hpp
#define screen_hpp

#include "canvas.hpp"

namespace toybox {
    
    /**
     A `screen_c` is an abstraction for displaying a screen of content.
     Contains an `image_c` for the bitmap data, and a `dirtymap_c` to restore
     dirty areas.
     */
    class screen_c : public canvas_c {
    public:
        screen_c(size_s screen_size = TOYBOX_SCREEN_SIZE_DEFAULT);
        ~screen_c();
        
        dirtymap_c *dirtymap() const { return _dirtymap; }

    private:
        image_c _image;
    };
    
}

#endif /* screen_hpp */
