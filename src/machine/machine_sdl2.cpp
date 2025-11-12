//
//  machine_host.cpp
//  toybox
//
//  Created by Fredrik on 2025-09-27.
//
#ifdef TOYBOX_HOST
#include "machine/machine.hpp"
#include "machine/host_bridge.hpp"
#include "media/screen.hpp"
#include "media/display_list.hpp"
#include "media/audio.hpp"
#include "machine/timer.hpp"
#include "SDL.h"
#include <atomic>
#include <mutex>
#include <libgen.h>
#include <unistd.h>

using namespace toybox;

class sdl2_host_bridge final : public host_bridge_c {
public:
    sdl2_host_bridge(machine_c &machine) : _machine(machine) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
        _window = SDL_CreateWindow("ToyBox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 400, SDL_WINDOW_SHOWN);
        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 320, 200);

        SDL_AudioSpec desired;
        SDL_zero(desired);
        desired.freq = 12517;                // STe/Falcon lowest compatible rate
        desired.format = AUDIO_S8;           // 8-bit signed audio (based on int8_t sample)
        desired.channels = 1;                // Mono audio
        desired.samples = 4096;              // Buffer size (can be tuned)
        desired.callback = nullptr;          // No callback, we'll use SDL_QueueAudio
        _device_id = SDL_OpenAudioDevice(nullptr, 0, &desired, nullptr, 0);
        
        set_shared(this);
    }
    ~sdl2_host_bridge() {
        if (_device_id) SDL_CloseAudioDevice(_device_id);
        if (_texture) SDL_DestroyTexture(_texture);
        if (_renderer) SDL_DestroyRenderer(_renderer);
        if (_window) SDL_DestroyWindow(_window);
        SDL_Quit();
        set_shared(nullptr);
    }
    
    virtual void yield() override {
        SDL_Delay(1);
    }

    virtual void pause_timers() override {
        _timer_mutex.lock();
    }
    
    virtual void resume_timers() override {
        _timer_mutex.unlock();
    }

    virtual void play(const sound_c &sound) override {
        // Get the audio sample data, length, and sample rate
        const int8_t* sample_data = sound.sample();
        uint32_t sample_length = sound.length();
        uint16_t sample_rate = sound.rate();
        
        if (!sample_data || sample_length == 0 || sample_rate == 0 || _device_id == 0) {
            return; // Invalid sound sample, exit early
        }
        
        // Unpause the audio device to start playback
        SDL_PauseAudioDevice(_device_id, 0);
        
        // Queue the audio sample for playback
        SDL_QueueAudio(_device_id, sample_data, sample_length);
    }

    void draw_display_list(const display_list_c *display) {
        std::lock_guard<std::recursive_mutex> lock(_timer_mutex);

        
        const image_c *active_image = nullptr;
        const palette_c *active_palette = nullptr;
        for (const auto& entry : *display) {
            switch (entry.item.display_type()) {
                case display_item_c::screen:
                    active_image = &entry.screen().image();
                    break;
                case display_item_c::palette:
                    active_palette = &entry.palette();
                    break;
                default:
                    hard_assert(false);
                    break;
            }
        }

        // Clear buffer to black
        struct  __packed color_s { uint8_t rgb[3]; uint8_t _; };
        color_s buffer[320 * 200];
        memset(buffer, 0, sizeof(color_s) * 320 * 200);
        
        if (active_image != NULL) {
            // If active image is set...
            {
                const auto size = active_image->size();
                hard_assert(size.width == 320 && size.height >= 200);
            }
            color_s palette[16] = { 0 };
            if (active_palette) {
                // If active palette is set
                for (int i = 0; i < 16; i++) {
                    (*active_palette)[i].get(&palette[i].rgb[0], &palette[i].rgb[1], &palette[i].rgb[2]);
                    buffer[i]._ = 0;
                }
            }
            point_s at;
            for (at.y = 0; at.y < 200; at.y++) {
                for (at.x = 0; at.x < 320; at.x++) {
                    const auto c = active_image->get_pixel(at);
                    if (c != image_c::MASKED_CIDX) {
                        auto offset = (at.y * 320 + at.x);
                        buffer[offset] = palette[c];
                    }
                }
            }
        }
        void *pixels;
        int pitch;
        SDL_LockTexture(_texture, nullptr, &pixels, &pitch);
        hard_assert(pitch / 4 == 320);
        memcpy(pixels, buffer, pitch * 200);
        SDL_UnlockTexture(_texture);
    }
    
    void vbl_interupt() {
        // Called on main thread by SDL timer
        std::lock_guard<std::recursive_mutex> lock(_timer_mutex);
        host_bridge_c::vbl_interupt();
    }

    void clock_interupt() {
        // Called on main thread by SDL timer
        std::lock_guard<std::recursive_mutex> lock(_timer_mutex);
        host_bridge_c::clock_interupt();
    }

    int run(machine_c::machine_f f) {
        static std::atomic<bool> s_should_quit{false};
        static int s_status = 0;
        struct Payload {
            int (*game_func)(machine_c &);
            machine_c *machine_inst;
        };
        Payload payload{f, &_machine};

        _thread = SDL_CreateThread(
            [](void *data) -> int {
                auto *p = static_cast<Payload *>(data);
                s_status = p->game_func(*p->machine_inst);
                s_should_quit.store(true);
                return s_status;
            },
            "GameThread",
            static_cast<void *>(&payload)
        );

        timer_c &vbl = timer_c::shared(timer_c::timer_e::vbl);
        _vbl_timer = SDL_AddTimer(1000 / vbl.base_freq(), vbl_cb, this);
        _clock_timer = SDL_AddTimer(5, clock_cb, this);

        while (!s_should_quit.load()) {
            SDL_Event event;
            const display_list_c *previous_display_list = nullptr;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        s_should_quit.store(true);
                        break;
                    case SDL_MOUSEMOTION:
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP: {
                        int x, y;
                        Uint32 buttons = SDL_GetMouseState(&x, &y);
                        bool left = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
                        bool right = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
                        host_bridge_c::shared().update_mouse(point_s(x / 2, y / 2), left, right);
                        break;
                    }
                    default:
                        break;
                }
            }
            pause_timers();
            auto display_list = _machine.active_display_list();
            if (display_list != previous_display_list) {
                draw_display_list(display_list);
                previous_display_list = display_list;
            }
            resume_timers();

            SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
            SDL_RenderClear(_renderer);
            SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
            SDL_RenderPresent(_renderer);
        }

        SDL_RemoveTimer(_vbl_timer);
        SDL_RemoveTimer(_clock_timer);

        SDL_DetachThread(_thread);
        _thread = nullptr;
        return s_status;
    }
    
private:
    SDL_Window *_window = nullptr;
    SDL_Renderer *_renderer = nullptr;
    SDL_Texture *_texture = nullptr;
    SDL_Thread *_thread = nullptr;
    SDL_AudioDeviceID _device_id;
    machine_c &_machine;
    std::recursive_mutex _timer_mutex;
    Uint32 _vbl_timer = 0;
    Uint32 _clock_timer = 0;

    static Uint32 vbl_cb(Uint32 interval, void *param) {
        static Uint64 last_tick = 0;
        Uint64 tick = SDL_GetTicks64();
        static_cast<sdl2_host_bridge *>(param)->vbl_interupt();
        
        auto &vbl = timer_c::shared(timer_c::timer_e::vbl);
        const Uint64 ideal_interval = (1000ULL * vbl.tick()) / vbl.base_freq();
        const Uint64 elapsed = tick - last_tick;
        last_tick = tick;
        interval = static_cast<Uint32>(ideal_interval - (tick - (1000ULL * vbl.tick() / vbl.base_freq())));
        interval = MIN(MAX(10, interval), 20);
        
        return interval;
    }
    static Uint32 clock_cb(Uint32 interval, void *param) {
        static_cast<sdl2_host_bridge *>(param)->clock_interupt();
        return interval;
    }
};

int machine_c::with_machine(int argc, const char * argv[], machine_f f) {
    assert(_shared_machine == nullptr && "Shared machine already initialized");
    char *dir = dirname((char *)argv[0]);
    hard_assert(chdir(dir) == 0);
    
    machine_c machine;
    _shared_machine = &machine;
    sdl2_host_bridge bridge(machine);
    int status = bridge.run(f);
    _shared_machine = nullptr;
    return status;
}

#endif
