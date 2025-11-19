//
//  viewport.cpp
//  toybox
//
//  Created by Fredrik on 2024-04-17.
//

#include "media/viewport.hpp"
#include "machine/machine.hpp"

using namespace toybox;


static size_s backing_size(size_s viewpost_size) {
    return size_s(
        // Width is min 320, and max 336 which is enough for HW scroll
        min(max(320, (viewpost_size.width + 15) & ~0xf), 336),
        // Height is nearest multiple of 16, + 8 for HW scroll
        ((viewpost_size.height + 15) & ~0xf) + 8
    );
}
// View port size is the total potential area, the image only what is needed to support hardware scrolling that area.
viewport_c::viewport_c(size_s viewport_size) :
    _viewport_size(viewport_size),
    _image(backing_size(viewport_size), false, nullptr),
    canvas_c(_image)
{
    assert(image().size().width >= 320 &&  image().size().width <= 336);
    assert(image().size().height >= 208);
    assert(_viewport_size.width > 0 && _viewport_size.width < 2048);  // To fit in fix16_t, about 6 screens wide
    assert(_viewport_size.height >= 200 && _viewport_size.height <= 208); // Only one screen height for now.
    _dirtymap = dirtymap_c::create(_image.size());
}

viewport_c::~viewport_c() {
    assert(_dirtymap && "Dirtymap must not be null");
    _free(_dirtymap);
}

void viewport_c::set_offset(point_s offset) {
    assert(offset.y == 0 && "Vertical offset must be 0");
    offset.x = max((int16_t)0, min((int16_t)(_viewport_size.width - 320), offset.x));
    const int16_t old_left_tile = _offset.x >> 4;
    const int16_t new_left_tile = offset.x >> 4;
    
    if (new_left_tile != old_left_tile) {
        const int16_t tile_delta = new_left_tile - old_left_tile;
        rect_s mark_rect(
            min(old_left_tile, new_left_tile) << 4, 0,
            abs(tile_delta) << 4, _image.size().height
        );
        if (tile_delta > 0) {
            // Scrolling right: unmark left, mark right
            _dirtymap->mark<false>(mark_rect);
            mark_rect.origin.x += 320;
            _dirtymap->mark<true>(mark_rect);
        } else {
            // Scrolling left: mark left, unmark right
            const int16_t left_tiles_gained = -tile_delta;
            _dirtymap->mark<true>(mark_rect);
            mark_rect.origin.x += 320;
            _dirtymap->mark<false>(mark_rect);
        }
        _clip_rect = rect_s(
            offset.x & ~0xf, 0,
            336, mark_rect.size.height
        );
    }
    _offset = offset;
}

const viewport_c::display_config_t viewport_c::display_config() const {
    uint8_t extra = _image.size().width > 320 ? 4 : 0;
    display_config_t config{
        _image._bitmap + ((_offset.x >> 4) << 2),// _bitmap_start
        (uint8_t)((_offset.x & 0xf) ? 0 : extra),// Add 4 extra words per line if shift is 0, if needed
        (uint8_t)(_offset.x & 0xf)               // Pixel shift
    };
    return config;
}
