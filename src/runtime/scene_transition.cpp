//
//  scene_transition.cpp
//  toybox
//
//  Created by Fredrik on 2024-03-23.
//

#include "runtime/scene.hpp"
#include "machine/machine.hpp"

using namespace toybox;

namespace toybox {
    
    // TODO: All of these transitions needs to be revisited now that we have display lists.
    
    class dither_transition_c : public transition_c {
    public:
        dither_transition_c(canvas_c::stencil_e dither) : transition_c(), _palette(nullptr) {
            _transition_state.full_restores_left = 2;
            _transition_state.type = canvas_c::effective_type(dither);
            _transition_state.shade = 0;
        }

        virtual void will_begin(const scene_c* from, const scene_c* to) override {
            /*
            if (to) {
                _palette = &to->configuration().palette;
                if (from) {
                    assert(_palette == &from->configuration().palette);
                }
            }
            */
        }

        virtual bool tick(int ticks) override {
            if (_transition_state.shade == 0 && _palette) {
                //machine_c::shared().set_active_palette(_palette);
                _palette = nullptr;
            }
            auto shade = MIN(canvas_c::STENCIL_FULLY_OPAQUE, _transition_state.shade);
            auto&phys_viewport = manager.display_list(scene_manager_c::display_list_e::front).get(PRIMARY_VIEWPORT).viewport();
            auto&log_viewport = manager.display_list(scene_manager_c::display_list_e::back).get(PRIMARY_VIEWPORT).viewport();
            phys_viewport.with_stencil(canvas_c::stencil(_transition_state.type, shade), [this, &phys_viewport, &log_viewport] {
                phys_viewport.draw_aligned(log_viewport.image(), log_viewport.clip_rect(), log_viewport.clip_rect().origin);
            });
            if (shade == canvas_c::STENCIL_FULLY_OPAQUE) {
                _transition_state.full_restores_left--;
            }
            _transition_state.shade += 1 + MAX(1, ticks);
            return _transition_state.full_restores_left <= 0;
        }
    protected:
        const palette_c* _palette;
        struct {
            int full_restores_left;
            canvas_c::stencil_e type;
            int shade;
        } _transition_state;
    };
}

class dither_through_transition_c final : public dither_transition_c {
public:
    dither_through_transition_c(canvas_c::stencil_e dither, uint8_t through) :
        dither_transition_c(dither), _palette(nullptr), _through(through) {
            _transition_state.full_restores_left = 4;
        }
    
    virtual bool tick(int ticks) override {
        if (_transition_state.full_restores_left > 2) {
            auto&phys_viewport = manager.display_list(scene_manager_c::display_list_e::front).get(PRIMARY_VIEWPORT).viewport();
            auto&log_viewport = manager.display_list(scene_manager_c::display_list_e::back).get(PRIMARY_VIEWPORT).viewport();
            auto shade = MIN(canvas_c::STENCIL_FULLY_OPAQUE, _transition_state.shade);
            phys_viewport.with_stencil(canvas_c::stencil(_transition_state.type, shade), [this, &phys_viewport, &log_viewport] {
                phys_viewport.fill(_through, rect_s(point_s(), phys_viewport.size()));
            });
            if (shade == canvas_c::STENCIL_FULLY_OPAQUE) {
                if (_palette) {
                   // machine_c::shared().set_active_palette(_palette);
                    _palette = nullptr;
                }
                _transition_state.full_restores_left--;
            }
            if (_transition_state.full_restores_left > 2) {
                _transition_state.shade += 1 + MAX(1, ticks);
            } else {
                _transition_state.shade = 0;
            }
            return false;
        } else {
            return dither_transition_c::tick(ticks);
        }
    }
protected:
    const palette_c* _palette;
    const uint8_t _through;
};

class fade_through_transition_c final : public transition_c {
public:
    fade_through_transition_c(color_c through) :
        transition_c(), _to_palette(nullptr), _through(through), _count(0)
    {}
    virtual ~fade_through_transition_c() {
        if (_to_palette) {
            //machine_c::shared().set_active_palette(_to_palette);
        }
    }
    virtual void will_begin(const scene_c* from, const scene_c* to) override {
        assert(to && "Target scene must not be null");
        uint8_t r, g, b;
        _through.get(&r, &g, &b);
        const palette_c from_palette;
        const palette_c to_palette;
        _to_palette = &to_palette;
        for (int i = 0; i <= 16; i++) {
            _palettes.emplace_back();
            auto&palette = _palettes.back();
            int shade = i * color_c::MIX_FULLY_OTHER / 16;
            for (int j = 0; j < 16; j++) {
                palette[j] = from_palette[j].mix(_through, shade);
            }
        }
        for (int i = 15; i >= 0; i--) {
            _palettes.emplace_back();
            auto&palette = _palettes.back();
            int shade = i * color_c::MIX_FULLY_OTHER / 16;
            for (int j = 0; j < 16; j++) {
                palette[j] = to_palette[j].mix(_through, shade);
            }
        }
    }
    virtual bool tick(int ticks) override {
        const int count = _count / 2;
        auto&m = machine_c::shared();
        if (count < 17) {
            //m.set_active_palette(&_palettes[count]);
        } else if (count < 18) {
            auto&phys_viewport = manager.display_list(scene_manager_c::display_list_e::front).get(PRIMARY_VIEWPORT).viewport();
            auto&log_viewport = manager.display_list(scene_manager_c::display_list_e::back).get(PRIMARY_VIEWPORT).viewport();
            phys_viewport.draw_aligned(log_viewport.image(), point_s());
        } else if (count < 34) {
            //m.set_active_palette(&_palettes[count - 1]);
        } else {
            return true;
        }
        _count++;
        return false;
    }
private:
    const palette_c* _to_palette;
    const color_c _through;
    int _count;
    vector_c<palette_c, 33> _palettes;
};

transition_c* transition_c::create(canvas_c::stencil_e dither) {
    return new dither_transition_c(dither);
}

transition_c* transition_c::create(canvas_c::stencil_e dither, uint8_t through) {
    return new dither_through_transition_c(dither, through);
}

transition_c* transition_c::create(color_c through) {
    return new fade_through_transition_c(through);
}
