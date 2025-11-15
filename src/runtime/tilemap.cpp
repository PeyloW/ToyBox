//
//  tilemap.cpp
//  toybox
//
//  Created by Fredrik on 2025-11-09.
//

#include "runtime/tilemap.hpp"

using namespace toybox;

tilemap_c::tilemap_c(const rect_s& bounds) :
    _tilespace_bounds(rect_s(
        bounds.origin.x / 16, bounds.origin.y / 16,
        bounds.size.width / 16, bounds.size.height / 16))
{
    _tiles.resize(_tilespace_bounds.size.width * _tilespace_bounds.size.height);
}
