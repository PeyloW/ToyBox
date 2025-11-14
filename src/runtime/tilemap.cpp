//
//  tilemap.cpp
//  toybox
//
//  Created by Fredrik on 2025-11-09.
//

#include "runtime/tilemap.hpp"

using namespace toybox;

tilemap_c::tilemap_c(const rect_s& bounds) :
    _tilespace_bounds(bounds)
{
    _tiles.resize(bounds.size.width * bounds.size.height);
}
