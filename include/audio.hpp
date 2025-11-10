//
//  audio.hpp
//  toybox
//
//  Created by Fredrik on 2024-02-18.
//

#pragma once

#include "cincludes.hpp"
#include "assets.hpp"
#include "types.hpp"
#include "memory.hpp"

namespace toybox {
    
    using namespace toybox;
    
    /**
     A `sound_c` is an 8 bit PCM sound sample.
     Sounds can be loaded for EA 85 AIFF files.
     */
    class sound_c final : public asset_c {
        friend class audio_mixer_c;
    public:
        sound_c(const char *path);
        virtual ~sound_c() {};
        
        __forceinline type_e asset_type() const override { return sound; }

        __forceinline const int8_t* sample() const { return _sample.get(); }
        __forceinline uint32_t length() const { return _length; }
        __forceinline uint16_t rate() const { return _rate; }
        
    private:
        unique_ptr_c<int8_t> _sample;
        uint32_t _length;
        uint16_t _rate;
    };
    
    /**
     A `music_c` is an abstract collection of music, containing one or more songs.
     TODO: Support mods.
     */
    class music_c : public asset_c {
        friend class audio_mixer_c;
    public:
        music_c() {};
        virtual ~music_c() {};

        __forceinline type_e asset_type() const override { return music; }
        
        virtual const char* title() const = 0;
        virtual const char* composer() const = 0;
        virtual int track_count() const = 0;
        virtual uint8_t replay_freq() const = 0;
    };
    
#if TOYBOX_TARGET_ATARI
    /**
     `ymmusic_c` is a concrete `music_c` representing YM-Music for Atari target.
     YB-music can be loaded from SNDH files.
     */
    class ymmusic_c final : public music_c {
        friend class audio_mixer_c;
    public:
        ymmusic_c(const char* path);
        virtual ~ymmusic_c() {};

        __forceinline const char* title() const override { return _title; }
        __forceinline const char* composer() const override  { return _composer; }
        __forceinline int track_count() const override { return _track_count; }
        __forceinline uint8_t replay_freq() const override { return _freq; }

    private:
        unique_ptr_c<uint8_t> _sndh;
        size_t _length;
        char* _title;
        char* _composer;
        int _track_count;
        uint8_t _freq;
#ifdef __M68000__
        uint16_t _music_init_code[8];
        uint16_t _music_exit_code[8];
        uint16_t _music_play_code[8];
#endif
    };
#endif

}
