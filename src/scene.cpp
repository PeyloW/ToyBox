//
//  scene.cpp
//  toybox
//
//  Created by Fredrik on 2024-03-01.
//

#include "scene.hpp"
#include "machine.hpp"
#include "algorithm.hpp"

using namespace toybox;

scene_c::scene_c() : manager(scene_manager_c::shared()) {}
scene_c::configuration_s scene_c::default_configuration = { size_s(320, 208), 2, true };
scene_c::configuration_s& scene_c::configuration() const {
    return default_configuration;
}

transition_c::transition_c() : manager(scene_manager_c::shared()) {}

class no_transition_c final : public transition_c {
public:
    no_transition_c() : transition_c() {
        _full_restores_left = 2;
    }
    virtual void will_begin(const scene_c *from, const scene_c *to) override {
    };
    virtual bool tick(int ticks) override {
        // TODO: Fix
        //phys_screen.draw_aligned(log_screen.image(), point_s());
        return --_full_restores_left <= 0;
    }
private:
    int _full_restores_left;
};

scene_manager_c::scene_manager_c() :
    vbl(timer_c::shared(timer_c::timer_e::vbl)),
    clock(timer_c::shared(timer_c::timer_e::clock))
{
    machine_c::shared();
    _overlay_scene = nullptr;
    _active_physical_screen = 0;
    _transition = nullptr;
    for (int i = 0; i < 3; i++) {
        auto& list = _display_lists.emplace_back();
        palette_c& pal = *new palette_c();
        screen_c& scr = *new screen_c(size_s(320, 208));
        list.emplace_front(PRIMARY_PALETTE, -1, pal);
        list.emplace_front(PRIMARY_SCREEN, -1, scr);
    }
    srand48(time(nullptr));
}

scene_manager_c& scene_manager_c::shared()
{
    static scene_manager_c s_shared;
    return s_shared;
}

#define DEBUG_NO_SET_SCREEN 0

void scene_manager_c::run(scene_c *rootscene, scene_c *overlayscene, transition_c *transition) {
    set_overlay_scene(overlayscene);
    push(rootscene, transition);

    vbl.reset_tick();
    int32_t previous_tick = vbl.tick();
    while (_scene_stack.size() > 0) {
        vbl.wait();
        int32_t tick = vbl.tick();
        int32_t ticks = tick - previous_tick;
        previous_tick = tick;
                
        if (_transition) {
            update_transition(ticks);
        } else {
            debug_cpu_color(DEBUG_CPU_TOP_SCENE_TICK);
            if (_scene_stack.size() > 0) {
                auto &scene = top_scene();
                auto &config = scene.configuration();
                // Merge dirty maps here!
                if (config.use_clear) {
                    auto &back_screen = update_clear();
                    back_screen.with_dirtymap(back_screen.dirtymap(), [&] {
                        update_scene(scene, ticks);
                        auto &clear = display_list(display_list_e::clear);
                        auto &back = display_list(display_list_e::back);
                        auto &fr_pal = clear.get(PRIMARY_PALETTE).palette();
                        auto &to_pal = back.get(PRIMARY_PALETTE).palette();
                        copy(fr_pal.begin(), fr_pal.end(), to_pal.begin());
                    });
                } else {
                    update_scene(scene, ticks);
                }
#if TOYBOX_DEBUG_RESTORE_SCREEN && DEBUG_DIRTYMAP
                physical_screen.dirtymap->debug("AF next");
#endif
            }
            _deletion_stack.clear();
        }
        debug_cpu_color(DEBUG_CPU_DONE);
        timer_c::with_paused_timers([&] {
            display_list_c &back = display_list(display_list_e::back);
            //sort(back.begin(), back.end());
            machine_c::shared().set_active_display_list(&back);
            swap_display_lists();
        });
    }
}

void scene_manager_c::set_overlay_scene(scene_c *overlay_scene) {
    if (_overlay_scene != overlay_scene) {
        if (_overlay_scene) {
            _overlay_scene->will_disappear(false);
        }
        _overlay_scene = overlay_scene;
        _overlay_scene->will_appear(false);
    }
}

void scene_manager_c::push(scene_c *scene, transition_c *transition) {
    scene_c *from = nullptr;
    if (_scene_stack.size() > 0) {
        from = &top_scene();
        from->will_disappear(true);
    }
    _scene_stack.push_back(scene);
    begin_transition(transition, from, scene, false);
}

void scene_manager_c::pop(transition_c *transition, int count) {
    scene_c *from = nullptr;
    while (count-- > 0) {
        from = &top_scene();
        from->will_disappear(false);
        enqueue_delete(from);
        _scene_stack.pop_back();
    }
    scene_c *to = nullptr;
    if (_scene_stack.size() > 0) {
        to = &top_scene();
    }
    begin_transition(transition, from, to, true);
}

void scene_manager_c::replace(scene_c *scene, transition_c *transition) {
    scene_c *from = &top_scene();
    from->will_disappear(false);
    enqueue_delete(from);
    _scene_stack.back() = scene;
    begin_transition(transition, from, scene, false);
}

display_list_c &scene_manager_c::display_list(display_list_e id) const {
    if (id == display_list_e::clear) {
        return (display_list_c&)_display_lists[2];
    } else {
        int idx = ((int)id + _active_physical_screen) & 0x1;
        return (display_list_c&)_display_lists[idx];
    }
}

void scene_manager_c::swap_display_lists() {
    _active_physical_screen = (_active_physical_screen + 1) & 0x1;
}


screen_c& scene_manager_c::update_clear() {
    screen_c *screens[3] = {
        &_display_lists[0].get(PRIMARY_SCREEN).screen(),
        &_display_lists[1].get(PRIMARY_SCREEN).screen(),
        &_display_lists[2].get(PRIMARY_SCREEN).screen(),
    };
    screens[0]->dirtymap()->merge(*screens[2]->dirtymap());
    screens[1]->dirtymap()->merge(*screens[2]->dirtymap());
    screens[2]->dirtymap()->clear();
    #if TOYBOX_DEBUG_RESTORE_SCREEN && DEBUG_DIRTYMAP
    screens[(_active_physical_screen + 1) & 0x1]->dirtymap->debug("log");
    screens[_active_physical_screen]->dirtymap->debug("phys");
    #endif
    screens[2]->dirtymap()->clear();
    debug_cpu_color(DEBUG_CPU_PHYS_RESTORE);
    auto &back_screen = *screens[(_active_physical_screen + 1) & 0x1];
    auto &clear = *screens[2];
    back_screen.dirtymap()->restore(back_screen, clear.image());
    return back_screen;
}

void scene_manager_c::update_scene(scene_c &scene, int32_t ticks) {
    debug_cpu_color(DEBUG_CPU_TOP_SCENE_TICK);
    display_list_c &back = display_list(display_list_e::back);
    scene.update(back, ticks);
    if (_overlay_scene) {
        debug_cpu_color(DEBUG_CPU_OVERLAY_SCENE_TICK);
        _overlay_scene->update(back, ticks);
    }
}

void scene_manager_c::begin_transition(transition_c *transition, const scene_c *from, scene_c *to, bool obsured) {
    if (to) {
        const auto &config = to->configuration();
        if (config.use_clear) {
            auto& clear = display_list(display_list_e::clear);
            auto& screen = clear.get(PRIMARY_SCREEN).screen();
            screen.with_dirtymap(screen.dirtymap(), [&]{
                to->will_appear(obsured);
            });
        } else {
            to->will_appear(obsured);
        }
    }
    if (_transition) delete _transition;
    if (transition) {
        _transition = transition;
    } else {
        _transition = new no_transition_c();
    }
    _transition->will_begin(from, to);
}

void scene_manager_c::update_transition(int32_t ticks) {
    assert(_transition && "Transition must not be null");
    debug_cpu_color(DEBUG_CPU_RUN_TRANSITION);
    bool done = _transition->tick(ticks);
    if (done) {
        end_transition();
    }
}

void scene_manager_c::end_transition() {
    assert(_transition && "Transition must not be null");
    delete _transition;
    _transition = nullptr;
}
