//
//  display_list.cpp
//  toybox
//
//  Created by Fredrik on 2025-10-18.
//

#include "media/display_list.hpp"
#include "machine/machine.hpp"

using namespace toybox;

display_list_c::~display_list_c() {
    auto& machine = machine_c::shared();
    if (machine.active_display_list() == this) {
        machine.set_active_display_list(nullptr);
    }
}
