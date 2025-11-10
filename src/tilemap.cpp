//
//  tilemap.cpp
//  toybox
//
//  Created by Fredrik on 2025-11-09.
//

#include "tilemap.hpp"
#include "iffstream.hpp"

using namespace toybox;

tilemap_c::tilemap_c(rect_s bounds) : _bounds(bounds) {
    int count = bounds.size.width * bounds.size.height;
    _tiles.reserve(count);
    while (--count) {
        _tiles.emplace_back();
    }
}

namespace cc4 {
    static constexpr cc4_t TLMP("TLMP"); // Tilemap
    static constexpr cc4_t TMHD("TMHD"); // Tilemap Header
    static constexpr cc4_t BODY("BODY"); // Tilemap body of tiles
}

struct __packed_struct tlmp_header_s {
    cc4_t id;
    rect_s bounds;          // Bounds of tilemap.
    uint16_t tile_size;     // Size of single tile in bytes, must be >= 4
};

/*
 File format is:
 Tilemap = FORM # { TLMP
    TMHD # { tlmp_header_s }
    BODY # { tile_size * (bounds.size.width * bounds.size.height) bytes }
    LIST # { TLMP
        Tilemap +
    } ?
 }
 */

tilemap_c::tilemap_c(const char *path, tile_loader_f loader) {
    iffstream_c file(path);
    iff_group_s form;
    if (!file.good() || !file.first(cc4::FORM, ::cc4::TLMP, form)) {
        hard_assert(0);
        return; // Not a ILBM
    }

}
