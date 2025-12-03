//
//  entity_helpers.hpp
//  toybox
//
//  Created by Fredrik on 2025-12-01.
//

#pragma once

#include "runtime/entity.hpp"
#include "runtime/tilemap_level.hpp"

namespace toybox {
    
    using resize_origin_e = directions_e;
    frect_s entity_position_with_frame_index(const tilemap_level_c& level, const entity_s& entity, uint8_t index, resize_origin_e origin = resize_origin_e::none);
    void set_frame_index(const tilemap_level_c& level, entity_s& entity, uint8_t index, resize_origin_e origin = resize_origin_e::none);
    
}
