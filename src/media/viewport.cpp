//
//  viewport.cpp
//  toybox
//
//  Created by Fredrik on 2024-04-17.
//

#include "media/viewport.hpp"
#include "machine/machine.hpp"

using namespace toybox;

// TODO: Viewport size should be separate from image size.
// View port size is the total potential area, the image only what is needed to support hardware scrolling that area.
viewport_c::viewport_c(size_s viewport_size) :
    _image(viewport_size, false, nullptr), canvas_c(_image)
{
    _dirtymap = dirtymap_c::create(_image.size());
    assert(viewport_size.width >= 320 && viewport_size.height >= 1 && "Viewport size must be at least 320x1");
}

viewport_c::~viewport_c() {
    assert(_dirtymap && "Dirtymap must not be null");
    _free(_dirtymap);
}
