//
//  entity.hpp
//  toybox_index
//
//  Created by Fredrik on 2025-11-11.
//

#pragma once

#include "geometry.hpp"

namespace toybox {
    

    class entity_c : public nocopy_c {
    public:
        entity_c() : _frame() {}
    private:
        frect_s _frame;
    };
    
}
