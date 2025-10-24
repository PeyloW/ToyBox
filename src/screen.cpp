//
//  screen.cpp
//  toybox
//
//  Created by Fredrik on 2024-04-17.
//

#include "screen.hpp"
#include "machine.hpp"

using namespace toybox;

screen_c::screen_c(size_s screen_size) :
    _image(screen_size, false, nullptr), canvas_c(_image)
{
    _dirtymap = create_dirtymap();
    assert(screen_size.width >= 320 && screen_size.height >= 1 && "Screen size must be at least 320x1");
}

screen_c::~screen_c() {
    assert(_dirtymap && "Dirtymap must not be null");
    _free(_dirtymap);
}
