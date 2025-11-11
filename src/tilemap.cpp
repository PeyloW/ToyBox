//
//  tilemap.cpp
//  toybox
//
//  Created by Fredrik on 2025-11-09.
//

#include "tilemap.hpp"
#include "iffstream.hpp"

using namespace toybox;

tilemap_c::tilemap_c(const rect_s& bounds) :
    _bounds(bounds), _entities(), _tiles(), _sub_tilemaps()
{
    _tiles.resize(bounds.size.width * bounds.size.height);
}

namespace cc4 {
    static constexpr cc4_t TLMP("TLMP"); // Tilemap
    static constexpr cc4_t TMHD("TMHD"); // Tilemap Header
    static constexpr cc4_t TILS("TILS"); // The tiles
    static constexpr cc4_t ENTS("ENTS"); // The entities
}

struct __packed_struct tlmp_header_s {
    cc4_t id;      // Map identifier
    rect_s bounds; // Bounds of tilemap.
};

/*
 File format is:
 Tilemap = FORM # { TLMP
    TMHD # { tlmp_header_s }
    TILS # { sizeof(tile_s) * (bounds.size.width * bounds.size.height) bytes }
    ENTS # { ?? bytes TODO: When we do AI } ?
    LIST # { TLMP
        Tilemap +
    } ?
 }
 */

tilemap_c::tilemap_c(const char *path) {
    iffstream_c file(path);
    iff_group_s form;
    if (!file.good() || !file.first(cc4::FORM, ::cc4::TLMP, form)) {
        hard_assert(0);
        return; // Not a ILBM
    }

}
