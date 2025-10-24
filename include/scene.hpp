//
//  scene.hpp
//  toybox
//
//  Created by Fredrik on 2024-03-01.
//

#pragma once

#include "timer.hpp"
#include "input.hpp"
#include "screen.hpp"
#include "display_list.hpp"
#include "vector.hpp"

namespace toybox {
    
#define DEBUG_CPU_RUN_TRANSITION 0x100
#define DEBUG_CPU_TOP_SCENE_TICK 0x030
#define DEBUG_CPU_PHYS_RESTORE 0x004
#define DEBUG_CPU_OVERLAY_SCENE_TICK 0x010
#define DEBUG_CPU_DONE 0x000
    
    using namespace toybox;
    
    class scene_manager_c;
        
    /**
     A `scene_c` is an abstraction for managing one screen of content.
     A scene is for example the menu, one level, or the hi-score table.
     */
    class scene_c : public nocopy_c {
    public:
        /**
         The `configuration_s` defines how to display and configures a `scene_c.`
         */
        struct configuration_s {
            const size_s screen_size;
            const int buffer_count;
            const bool use_clear;
        };
        scene_c();
        virtual ~scene_c() {};
        
        virtual configuration_s &configuration() const;
        static configuration_s default_configuration;
        
        virtual void will_appear(bool obsured) {};
        virtual void will_disappear(bool obscured) {};
        
        virtual void update(display_list_c &display_list, int ticks) {};
        
    protected:
        scene_manager_c &manager;
    };
    
    /**
     A `transition_c` manages the visual transition from one scene to another.
     */
    class transition_c : public nocopy_c {
    public:
        transition_c();
        virtual ~transition_c() {}
        
        // TODO: Must configure using from and to display lists.
        virtual void will_begin(const scene_c *from, const scene_c *to) = 0;
        virtual bool tick(int ticks) = 0;
        
        static transition_c *create(canvas_c::stencil_e dither);
        static transition_c *create(canvas_c::stencil_e dither, uint8_t through);
        static transition_c *create(color_c through);
    
    protected:
        scene_manager_c &manager;
    };
        
    /**
     The `scene_manager_c` handles a stack of scenes, and a set of screens.
     The top-most scene is the active scene currently displayed.
     The optional overlay scene is always handled ontop of the top-most scene,
     and can be used for handling a persistent mouse cursor, or status bar.
     The screens are the front screen being displayed, the back screen being
     drawn, and optionally the clear screen used for fast restoration of the
     other screens.
     As the top-most scene changes the manager creates a transition to handle
     the visual transition.
     */
    class scene_manager_c final : public nocopy_c {
    public:
        enum class display_list_e : int8_t {
            clear = -1, front, back
        };
        static scene_manager_c& shared();
        
        void run(scene_c *rootscene, scene_c *overlay_scene = nullptr, transition_c *transition = nullptr);
        
        void set_overlay_scene(scene_c *overlay_cene);
        __forceinline scene_c *overlay_scene() const { return _overlay_scene; };

        __forceinline scene_c &top_scene() const {
            return *_scene_stack.back();
        };
        void push(scene_c *scene, transition_c *transition = transition_c::create(color_c()));
        void pop(transition_c *transition  = transition_c::create(color_c()), int count = 1);
        void replace(scene_c *scene, transition_c *transition = transition_c::create(canvas_c::stencil_e::random));
        
        timer_c &vbl;
        timer_c &clock;
        
        display_list_c &display_list(display_list_e id) const;
        
    private:
        scene_manager_c();
        ~scene_manager_c() = default;

        transition_c *_transition;
        scene_c *_overlay_scene;
        vector_c<scene_c *, 8> _scene_stack;
        vector_c<unique_ptr_c<scene_c>, 8> _deletion_stack;
        
        void swap_display_lists();
        
        inline screen_c& update_clear();
        inline void update_scene(scene_c &scene, int32_t ticks);

        __forceinline void enqueue_delete(scene_c *scene) {
            _deletion_stack.emplace_back(scene);
        }
        inline void begin_transition(transition_c *transition, const scene_c *from, scene_c *to, bool obsured);
        inline void update_transition(int32_t ticks);
        inline void end_transition();

        vector_c<display_list_c, 3> _display_lists;
        int _active_physical_screen;
    };
    
}
