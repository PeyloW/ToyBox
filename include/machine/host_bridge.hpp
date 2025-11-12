//
//  system_host.hpp
//  toybox
//
//  Created by Fredrik on 2024-02-11.
//

#ifdef __M68000__
#error "For host machine only"
#else

#pragma once

#include "core/geometry.hpp"

namespace toybox {
    
    using namespace toybox;
    
    class sound_c;
    
    /**
     A `host_bridge_c` is the abstraction needed for running a target emulated
     on a modern host machine, such as macOS.
     */
    class host_bridge_c : nocopy_c {
    public:
        static host_bridge_c& shared();
        static void set_shared(host_bridge_c *bridge);

        // Host must call on a 50/60Hz interval
        void vbl_interupt();

        // Host must call on a 200Hz interval
        void clock_interupt();
                
        // Host must call when mouse state changes
        void update_mouse(point_s position, bool left, bool right);
                
        // Host must provide a yield function
        virtual void yield() = 0;

        virtual void pause_timers() = 0;
        virtual void resume_timers() = 0;

        // Host should provide a play function
        virtual void play(const sound_c &sound) {};
        
    private:
        
    };
    
}

#endif
